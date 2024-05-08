#define MAX_LINES NUM_RINGS * 2

class LineSubPattern : public SubPattern {
private:
  Line _lines[MAX_LINES];
  uint8_t _numLines = MAX_LINES;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showRotatingPong() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isOutOfBounds()) {
        _lines[i].reverse();
      }
      _lines[i].show();
    }
  }

  void _showLasers() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isFullyOutOfBounds()) {
        _lines[i].setPosition(MAX_DEPTH + _lines[i].getLength());
      }
      _lines[i].show();
    }
  }

  void _showRainfall() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isFullyOutOfBounds()) {
        if (_lines[i].isReversed()) {
          _lines[i].setPosition(NUM_LEDS_PER_RING / 2 + _lines[i].getLength());
        } else {
          _lines[i].setPosition(0);
        }
      }
      _lines[i].show();
    }
  }

public:
  static const uint8_t ROTATING_PONG = 0;
  static const uint8_t LASERS = 1;
  static const uint8_t RAINFALL = 2;

  LineSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case ROTATING_PONG:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].setPosition(i * (MAX_DEPTH / NUM_STRAIGHTS));
      }
      break;
    case LASERS:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].reverse();
        _lines[i].setPosition(MAX_DEPTH - i * (MAX_DEPTH / NUM_STRAIGHTS));
      }
      break;
    case RAINFALL:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        if (i % 2 == 0) {
          int offset = rings[i].offset;
          Path path = {&leds[offset], &ledAngle[offset], &ledDepth[offset],
                       NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setPosition(NUM_LEDS_PER_RING / 2 + _lines[i].getLength());
          _lines[i].reverse();
        } else {
          int offset = rings[i].offset + NUM_LEDS_PER_RING / 2;
          Path path = {&leds[offset], &ledAngle[offset], &ledDepth[offset],
                       NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setPosition(0);
        }
      }
      break;
    default:
      break;
    }
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    for (uint8_t i = 0; i < _numLines; i++) {
      _lines[i].setPercentBrightness(percentBrightness);
    }
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case ROTATING_PONG:
      _showRotatingPong();
      break;
    case LASERS:
      _showLasers();
      break;
    case RAINFALL:
      _showRainfall();
      break;
    default:
      break;
    }
  }
};
