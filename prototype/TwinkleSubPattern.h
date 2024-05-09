uint8_t nodeBrightness[NUM_STRAIGHTS * NUM_RINGS];

class TwinkleSubPattern : public SubPattern {
private:
  Twinkle _twinkle;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showTwinkleGroups() { _twinkle.showGroups(ANGLE_SEGMENT_LENGTH); }

public:
  static const uint8_t TWINKLE = 0;
  static const uint8_t TWINKLE_GROUPS = 1;

  TwinkleSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case TWINKLE_GROUPS:
      _twinkle.setDensity(Twinkle::DENSITY.MIN);
      break;
    default:
      break;
    }
  }

  void setSpeed(uint8_t speed) { _twinkle.setSpeed(speed); }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    _twinkle.setPercentBrightness(percentBrightness);
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case TWINKLE_GROUPS:
      _showTwinkleGroups();
      break;
    default:
      _twinkle.showLEDs();
      break;
    }
  }
};
