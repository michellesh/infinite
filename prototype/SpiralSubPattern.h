#define MAX_SPIRALS 3

class SpiralSubPattern : public SubPattern {
private:
  Spiral _spirals[MAX_SPIRALS];
  uint8_t _numSpirals = MAX_SPIRALS;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showBasicSpiralRotation() {
    _spirals[0].show();
    _spirals[1].show();
  }

public:
  static const uint8_t BASIC_SPIRALS = 0;

  SpiralSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
    switch (_activeSubPattern) {
    case BASIC_SPIRALS:
      _numSpirals = 2;
      _spirals[0] = Spiral(1);
      _spirals[1] = Spiral(2);
      _spirals[0].setOffset(3);
      _spirals[1].setOffset(-3);
      _spirals[0].setSpeed(5);
      _spirals[1].setSpeed(5);
      _spirals[0].setWidth(180);
      _spirals[1].setWidth(180);
      break;
    default:
      break;
    }
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    for (uint8_t i = 0; i < _numSpirals; i++) {
      _spirals[i].setPercentBrightness(percentBrightness);
    }
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case BASIC_SPIRALS:
      _showBasicSpiralRotation();
      break;
    default:
      break;
    }
  }
};
