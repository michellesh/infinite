class Line : public Pattern {
private:
  uint8_t _id = 0;
  uint8_t _fadeType = FADE_HEAVY;
  float _speedMultiplier = 1;
  float _lengthMultiplier = 1;
  float _prevPosition = 0;
  float _position = 0;
  int _offset = 0;
  bool _reverse = reverse;
  Path _path;

  uint8_t _getBrightness(int indexOnPath, int indexOnLine) {
    int _length = getLength();
    if (_fadeType == FADE_COMET) {
      uint8_t _twinkleBrightness =
          twinkleBrightness[_path.offset + indexOnPath];
      float t = (float)_twinkleBrightness * mapf(indexOnLine, 0, _length, 0, 1);
      if (indexOnLine > _length * 3 / 4) {
        float b = mapf(indexOnLine, _length * 3 / 4, _length, 0, 255);
        t = max(t, b);
      }
      return t;
    } else if (_fadeType == FADE_LIGHT) {
      int fadeLengthPercent = 10; // the last 10% on each end fade from 255 to 0
      int percent = (float)indexOnLine / (float)_length * 100;
      percent = percent > fadeLengthPercent ? 100 - percent : percent;
      return mapc(percent, 0, fadeLengthPercent, 0, 255);
    }

    // FADE_HEAVY
    return addFadeShape(map(indexOnLine, 0, _length, 0, 255));
  }

  void _show() {
    if (_fadeType == FADE_COMET) {
      twinkleSome(NUM_LEDS, 5, 6);
    }

    // show this Line at current position and add tail of length
    for (int indexOnLine = 0; indexOnLine < getLength(); indexOnLine++) {
      int indexOnPath =
          _position - indexOnLine; // tail extends backwards behind position
      if (indexOnPath > 0 && indexOnPath < _path.length) {
        uint8_t brightness = _getBrightness(indexOnPath, indexOnLine);
        _path.leds[indexOnPath] =
            palette.getColor(_path.offset + indexOnPath).nscale8(brightness);
      }
    }
  }

public:
  Line(uint8_t id = 0) { _id = id; }

  static constexpr uint8_t FADE_HEAVY = 0;
  static constexpr uint8_t FADE_COMET = 1;
  static constexpr uint8_t FADE_LIGHT = 2;

  static constexpr Range LENGTH = {
      DEPTH_SEGMENT_LENGTH, DEPTH_SEGMENT_LENGTH * 6, DEPTH_SEGMENT_LENGTH * 2};

  void setSpeedMultiplier(float speedMultiplier) {
    _speedMultiplier = speedMultiplier;
  }

  void setLengthMultiplier(float lengthMultiplier) {
    _lengthMultiplier = lengthMultiplier;
  }

  int getLength() {
    return map(width, 1, 10, LENGTH.MIN, LENGTH.MAX) * _lengthMultiplier;
  }

  int getEndPosition(bool includeLineLength = true) {
    int endPosition = _path.length;
    if (includeLineLength) {
      endPosition += getLength();
    }
    return endPosition;
  }

  float getPosition(bool includeLineLength = true) {
    int endPosition = getEndPosition(includeLineLength);
    float newPosition = isReversed()
                            ? mapBeat(0, endPosition, _speedMultiplier)
                            : mapBeat(endPosition, 0, _speedMultiplier);
    if (isReversed()) {
      newPosition += _offset;
      newPosition = (int)newPosition % endPosition;
    } else {
      newPosition -= _offset;
      newPosition += endPosition * 2;
      newPosition = (int)newPosition % endPosition;
    }
    return newPosition;
  }

  void setPath(Path path) { _path = path; }

  void setOffset(int offset) { _offset = offset; }

  Path getPath() { return _path; }

  bool isReversed() { return _reverse ^ reverse; } // XOR

  void setReverse(bool r) { _reverse = r; }

  void toggleReverse() { _reverse = !_reverse; }

  void setFadeType(uint8_t fadeType) { _fadeType = fadeType; }

  void resetPrevPosition() { _prevPosition = 0; }

  bool isFullyOutOfBounds(float position) {
    return (!isReversed() && position > _prevPosition) ||
           (isReversed() && position < _prevPosition);
  }

  bool isFullyOutOfBounds() { return isFullyOutOfBounds(_position); }

  void ignoreNewPosition() { _position = _prevPosition; }

  void commitNewPosition() { _prevPosition = _position; }

  void showPathFixed(int maxTwinkles, bool isFlickering, bool flickerState) {
    int s = (float)speed * _speedMultiplier;
    twinkleSome(maxTwinkles, min(s, MAX_TWINKLE_SPEED),
                map(density, 1, 10, 1, MAX_TWINKLE_DENSITY));

    // No position, just shows entire path
    for (int i = 0; i < _path.length; i++) {
      uint8_t brightness = isFlickering ? (flickerState ? 255 : 0)
                           : twinkleBrightness[_id] < 127 ? 0
                                                          : 255;
      _path.leds[i] = palette.getColor(_path.offset + i).nscale8(brightness);
    }
  }

  void show(float nextPosition) {
    _show();
    _position = nextPosition;
  }

  void show() {
    _show();
    _position = getPosition();
  }

  void showRepeat() {
    int _length = getLength();
    int numLinesOnPath = floor(_path.length / (_length * 2));
    int increment = _path.length / numLinesOnPath;
    for (int indexOnPath = 0; indexOnPath < _path.length;
         indexOnPath += increment) {
      for (int indexOnLine = 0; indexOnLine < getLength(); indexOnLine++) {
        int index = indexOnPath + indexOnLine + _position;
        if (index > _path.length)
          index -= _path.length;
        if (index >= 0 && index < _path.length) {
          CRGB color = palette.getColor(_path.offset + indexOnPath);
          uint8_t distInGroup = map(indexOnLine, 0, _length, 0, 255);
          _path.leds[index] = color.nscale8(basicFade(distInGroup));
        }
      }
    }

    _position = getPosition(false);
  }
};
