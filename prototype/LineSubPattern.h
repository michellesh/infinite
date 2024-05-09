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
        // TODO only works if reversed
        _lines[i].setPosition(MAX_DEPTH + _lines[i].getLength());
      }
      _lines[i].show();
    }
  }

  void _showRainfall() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isFullyOutOfBounds()) {
        int r = random(0, 50);
        if (_lines[i].isReversed()) { // even numbered rings
          int max = NUM_LEDS_PER_RING / 2 + _lines[i].getLength();
          _lines[i].setPosition(max + r);
        } else {
          _lines[i].setPosition(-r);
        }
      }
      _lines[i].show();
    }
  }

  void _showBasketWeaving() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isFullyOutOfBounds()) {
        // TODO only works if reversed
        Path path = _lines[i].getPath();
        _lines[i].setPosition(path.length + _lines[i].getLength());
      }
      _lines[i].show();
    }
  }

  void _showCometTrails() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isFullyOutOfBounds()) {
        // TODO only works if reversed
        _lines[i].setPosition(MAX_DEPTH + _lines[i].getLength());
      }
      _lines[i].show();
    }
  }

  void _showRotatingHexagons() {
    for (int i = 0; i < _numLines; i++) {
      if (_lines[i].isOutOfBounds()) {
        if (_lines[i].isReversed()) {
          Path path = _lines[i].getPath();
          _lines[i].setPosition(path.length + _lines[i].getLength());
        } else {
          _lines[i].setPosition(0);
        }
      }
      _lines[i].showRepeat();
    }
  }

public:
  static const uint8_t ROTATING_PONG = 0;
  static const uint8_t LASERS = 1;
  static const uint8_t RAINFALL = 2;
  static const uint8_t BASKET_WEAVING = 3;
  static const uint8_t COMET_TRAILS = 4;
  static const uint8_t ROTATING_HEXAGONS = 5;
  static const uint8_t COUNTER_ROTATING_HEXAGONS = 6;
  static const uint8_t VARIABLE_SPEED_ROTATION = 7;

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
        _lines[i].setSpeedMultiplier(0.5);
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
    case BASKET_WEAVING:
      _numLines = NUM_STRAIGHTS + NUM_RINGS;
      for (uint8_t i = 0; i < NUM_STRAIGHTS; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].reverse();
        _lines[i].setPosition(MAX_DEPTH - i * (MAX_DEPTH / NUM_STRAIGHTS
              ));
      }
      for (uint8_t i = NUM_STRAIGHTS; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.5);
        _lines[i].setPath(rings[i - NUM_STRAIGHTS]);
        _lines[i].reverse();
        _lines[i].setPosition(360 - (i - NUM_STRAIGHTS) * (360 / NUM_RINGS));
        // TODO this shouldn't be 360
        // although that might be why it starts with a delay which is neat
      }
      break;
    case COMET_TRAILS:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.2);
        _lines[i].setPath(straights[i]);
        _lines[i].setLength(MAX_DEPTH / 3);
        _lines[i].setFadeType(Line::FADE_COMET);
        _lines[i].reverse();
        _lines[i].setPosition(MAX_DEPTH - i * (MAX_DEPTH / NUM_STRAIGHTS));
      }
      break;
    case ROTATING_HEXAGONS:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.5);
        _lines[i].setPath(rings[i]);
        _lines[i].setLength(NUM_LEDS_PER_RING / 16); // TODO dont hardcode 16?
        _lines[i].setPosition(i * (10 * NUM_LEDS_PER_RING / 360)); // 10 "degrees"
      }
      break;
    case COUNTER_ROTATING_HEXAGONS:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.5);
        _lines[i].setPath(rings[i]);
        _lines[i].setLength(NUM_LEDS_PER_RING / 16); // TODO dont hardcode 16?
        if (i % 2 == 0) {
          _lines[i].reverse();
        }
        _lines[i].setPosition(i * (10 * NUM_LEDS_PER_RING / 360)); // 10 "degrees"
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
    case BASKET_WEAVING:
      _showBasketWeaving();
      break;
    case COMET_TRAILS:
      _showCometTrails();
      break;
    case ROTATING_HEXAGONS:
      _showRotatingHexagons();
      break;
    case COUNTER_ROTATING_HEXAGONS:
      _showRotatingHexagons();
      break;
    default:
      break;
    }
  }
};
