#define MAX_SPIRALS 3

class SpiralSubPattern : public SubPattern {
private:
  Spiral _spirals[MAX_SPIRALS];
  uint8_t _numSpirals = MAX_SPIRALS;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showSingleSpiral() { _spirals[0].show(); }

  void _showDoubleSpiral() {
    _spirals[0].show();
    _spirals[1].show();
  }

public:
  static const uint8_t SINGLE_SPIRAL = 0;
  static const uint8_t DOUBLE_SPIRAL = 1;

  SpiralSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
    switch (_activeSubPattern) {
    case SINGLE_SPIRAL:
      _numSpirals = 1;
      _spirals[0] = Spiral(1);
      _spirals[0].setColorPaletteIndex(0);
      _spirals[0].setSpeedMultiplier(1.5);
      break;
    case DOUBLE_SPIRAL:
      _numSpirals = 2;
      _spirals[0] = Spiral(1);
      _spirals[1] = Spiral(2);
      _spirals[0].setColorPaletteIndex(0);
      _spirals[1].setColorPaletteIndex(170);
      _spirals[1].setDensityMultiplier(-1);
      _spirals[0].setSpeedMultiplier(1.5);
      _spirals[1].setSpeedMultiplier(1.5);
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
    case SINGLE_SPIRAL:
      _showSingleSpiral();
      break;
    case DOUBLE_SPIRAL:
      _showDoubleSpiral();
      break;
    default:
      break;
    }
  }
};
