class Line : public Pattern {
private:
  uint8_t _id = 0;
  int _length = LENGTH.DFLT;
  bool _reverse = false;
  float _speedMultiplier = 1;
  float _position = 0;
  Path _path;

  void _updatePosition() {
    _position += (speed * _speedMultiplier) * (_reverse ? -1 : 1);
  }

public:
  Line(uint8_t id = 0) { _id = id; }

  static constexpr Range SPEED = {1, 10, 3};
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

  void setPath(Path &path) { _path = path; }

  bool isReversed() { return _reverse; }

  void reverse() { _reverse = !_reverse; }

  bool isOutOfBounds() {
    return (_position >= _path.length && !_reverse) ||
           (_position < _length && _reverse);
  }

  bool isFullyOutOfBounds() {
    return (_position >= _path.length + _length && !_reverse) ||
           (_position < 0 && _reverse);
  }

  void show() {
    // show this Line at current position and add tail of length _length
    for (int i = 0; i < _length; i++) {
      int index = _position - i; // tail extends backwards behind position
      if (index > 0 && index < _path.length) {
        int brightness = addFadeShape(map(i, 0, _length, 0, 255));
        _path.leds[index] =
            palette.getColor(_path.offset + index).nscale8(brightness);
      }
    }

    _updatePosition();
  }
};
