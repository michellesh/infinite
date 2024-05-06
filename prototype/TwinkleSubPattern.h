class TwinkleSubPattern : public SubPattern {
 private:
  Twinkle _twinkle;
  uint8_t _percentBrightness = 0;  // percent brightness of the whole pattern

 public:
  static const uint8_t LOW_DENSITY = 0;
  static const uint8_t MEDIUM_DENSITY = 1;
  static const uint8_t HIGH_DENSITY = 2;

  TwinkleSubPattern(uint8_t activeSubPattern = 0) {
    switch (activeSubPattern) {
      case LOW_DENSITY:
        _twinkle.setDensity(Twinkle::DENSITY.MIN);
        break;
      case MEDIUM_DENSITY:
        _twinkle.setDensity(Twinkle::DENSITY.DFLT);
        break;
      case HIGH_DENSITY:
        _twinkle.setDensity(Twinkle::DENSITY.MAX);
        break;
      default:
        break;
    }
  }

  void setSpeed(uint8_t speed) {
    _twinkle.setSpeed(speed);
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    _twinkle.setPercentBrightness(percentBrightness);
  }

  virtual void show() { _twinkle.show(); }
};
