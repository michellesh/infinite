class TwinkleSubPattern : public SubPattern {
private:
  Twinkle _twinkle;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

public:
  static const uint8_t TWINKLE = PATTERN_TWINKLE;
  static const uint8_t RANDOM_FADING_SEGMENTS = PATTERN_RANDOM_FADING_SEGMENTS;
  static const uint8_t TWINKLE_OVERLAY = PATTERN_TWINKLE_OVERLAY;

  TwinkleSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setActivePattern(uint8_t activeSubPattern) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case TWINKLE:
      _twinkle.reset();
      break;
    case RANDOM_FADING_SEGMENTS:
      _twinkle.setDensityMultiplier(0.5);
      _twinkle.setSpeedMultiplier(2);
      _twinkle.setWidthMultiplier(DEPTH_SEGMENT_LENGTH / 2);
      _twinkle.setWithOverlay(false);
      break;
    case TWINKLE_OVERLAY:
      _twinkle.reset();
      _twinkle.setWithOverlay(true);
      break;
    default:
      break;
    }
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    _twinkle.setPercentBrightness(percentBrightness);
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case TWINKLE:
    case RANDOM_FADING_SEGMENTS:
    case TWINKLE_OVERLAY:
      _twinkle.show();
      break;
    default:
      break;
    }
  }
};
