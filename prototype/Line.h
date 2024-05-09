class Line : public Pattern {
private:
  uint8_t _id = 0;
  int _length = LENGTH.DFLT;
  uint8_t _fadeType = FADE_BOTH_ENDS;
  float _speedMultiplier = 1;
  float _position = 0;
  Path _path;

  float _getSpeed() { return (float)speed * _speedMultiplier; }

  void _updatePosition() { _position = _position + _getSpeed(); }

  uint8_t _getBrightness(int indexOnPath, int indexOnLine) {
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

  void setPosition(float position) { _position = position; }

  float getPosition() { return _position; }

  void setLength(int length) { _length = length; }

  int getLength() { return _length; }

  void setPath(Path &path) { _path = path; } // TODO remove &?

  Path getPath() { return _path; }

  bool isReversed() { return _speedMultiplier < 0; }

  void reverse() { _speedMultiplier = _speedMultiplier * -1; }

  void setFadeType(uint8_t fadeType) { _fadeType = fadeType; }

  bool isOutOfBounds() {
    return (_position >= _path.length && !isReversed()) ||
           (_position < _length && isReversed());
  }

  bool isFullyOutOfBounds() {
    return (_position >= _path.length + _length && !isReversed()) ||
           (_position < 0 && isReversed());
  }

  void show() {
    // show this Line at current position and add tail of length _length
    for (int indexOnLine = 0; indexOnLine < _length; indexOnLine++) {
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
    for (int indexOnPath = 0; indexOnPath < _path.length; indexOnPath++) {
      if ((int)floor((indexOnPath + _position) / _length) % 2 == 0) {
        int indexOnLine = (int(indexOnPath + _position) % _length);
        CRGB color = palette.getColor(_path.offset + indexOnPath);
        uint8_t distInGroup = map(indexOnLine, 0, _length, 0, 255);
        _path.leds[indexOnPath] = color.nscale8(basicFade(distInGroup));
      }
    }

    _updatePosition();
  }
};
