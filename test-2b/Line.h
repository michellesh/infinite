class Line : public Pattern {
private:
  uint8_t _id = 0;
  uint8_t _fadeType = FADE_BOTH_ENDS;
  float _speedMultiplier = 1;
  float _lengthMultiplier = 1;
  float _position = 0;
  Path _path;

  float _getSpeed() {
    return (float)speed * _speedMultiplier * (reverse ? -1 : 1);
  }

  void _updatePosition() { _position = _position + _getSpeed(); }

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
    }

    // FADE_BOTH_ENDS
    return addFadeShape(map(indexOnLine, 0, _length, 0, 255));
  }

public:
  Line(uint8_t id = 0) { _id = id; }

  static constexpr uint8_t FADE_BOTH_ENDS = 0;
  static constexpr uint8_t FADE_COMET = 1;

  static constexpr Range LENGTH = {
      DEPTH_SEGMENT_LENGTH, DEPTH_SEGMENT_LENGTH * 3, DEPTH_SEGMENT_LENGTH * 2};

  uint8_t getId() { return _id; }

  void setSpeedMultiplier(float speedMultiplier) {
    _speedMultiplier = speedMultiplier;
  }

  void setLengthMultiplier(float lengthMultiplier) {
    _lengthMultiplier = lengthMultiplier;
  }

  int getLength() {
    return map(width, 1, 10, LENGTH.MIN, LENGTH.MAX) * _lengthMultiplier;
  }

  void setPosition(float position) { _position = position; }

  float getPosition() { return _position; }

  void setPath(Path path) { _path = path; }

  Path getPath() { return _path; }

  bool isReversed() { return _speedMultiplier * (reverse ? -1 : 1) < 0; }

  void setReverse(bool reverse) {
    _speedMultiplier =
        reverse ? abs(_speedMultiplier) * -1 : abs(_speedMultiplier);
  }

  void toggleReverse() { _speedMultiplier *= -1; }

  void setFadeType(uint8_t fadeType) { _fadeType = fadeType; }

  bool isOutOfBounds() {
    return (_position >= _path.length && !isReversed()) ||
           (_position < getLength() && isReversed());
  }

  bool isFullyOutOfBounds() {
    return (_position >= _path.length + getLength() && !isReversed()) ||
           (_position < 0 && isReversed());
  }

  void resetPosition() {
    setPosition(isReversed() ? _path.length + getLength() : 0);
  }

  void show() {
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

    _updatePosition();
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

    _updatePosition();
  }
};
