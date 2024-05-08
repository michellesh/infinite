class Line : public Pattern {
private:
  uint8_t _id = 0;
  int _length = LENGTH.DFLT;
  float _speed = SPEED.DFLT;
  float _position = 0;
  Path _path;

  void _updatePosition() {
    _position += _speed;
    if (_position >= _path.length || (_position < _length && _speed < 0)) {
      _speed = -_speed;
    }
  }

public:
  Line(uint8_t id = 0) { _id = id; }

  static constexpr Range SPEED = {1, 10, 3};
  static constexpr Range LENGTH = {
      DEPTH_SEGMENT_LENGTH, DEPTH_SEGMENT_LENGTH * 3, DEPTH_SEGMENT_LENGTH * 2};

  void setPosition(float position) { _position = position; }

  void setSpeed(float speed) { _speed = speed; }

  void setPath(Path &path) { _path = path; }

  void show() {
    // show this Line at current position and add tail of length _length
    for (int i = _position; i > _position - _length && i > 0; i--) {
      int brightness =
          addFadeShape(map(i, _position, _position - _length, 0, 255));
      _path.leds[i] = palette.getColor(_path.offset + i).nscale8(brightness);
    }

    _updatePosition();
  }
};
