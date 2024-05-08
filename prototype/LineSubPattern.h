#define MAX_LINES 10

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
      if (_lines[i].isOutOfBounds()) {
        _lines[i].setPosition(MAX_DEPTH);
      }
      _lines[i].show();
    }
  }

public:
  static const uint8_t ROTATING_PONG = 0;
  static const uint8_t LASERS = 1;

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
    default:
      break;
    }
  }
};
