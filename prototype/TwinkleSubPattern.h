class TwinkleSubPattern : public SubPattern {
private:
  Twinkle _twinkle;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showTwinkle() { _twinkle.show(); }

  void _showRandomFadingSegments() { _twinkle.show(); }

public:
  static const uint8_t TWINKLE = 0;
  static const uint8_t RANDOM_FADING_SEGMENTS = 1;

  TwinkleSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case TWINKLE:
      break;
    case RANDOM_FADING_SEGMENTS:
      _twinkle.setDensityMultiplier(0.5);
      _twinkle.setSpeedMultiplier(2);
      _twinkle.setWidthMultiplier(DEPTH_SEGMENT_LENGTH / 2);
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
      _showTwinkle();
      break;
    case RANDOM_FADING_SEGMENTS:
      _showRandomFadingSegments();
      break;
    default:
      break;
    }
  }
};
