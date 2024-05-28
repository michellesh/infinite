class FlashSubPattern : public SubPattern {
private:
  Flash _flash;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

public:
  static const uint8_t FLASHING_HEXAGONS = PATTERN_FLASHING_HEXAGONS;
  static const uint8_t FLASHING_HEXAGONS_WARP = PATTERN_FLASHING_HEXAGONS_WARP;

  FlashSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setActivePattern(uint8_t activeSubPattern) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case FLASHING_HEXAGONS:
      _flash.reset();
      break;
    case FLASHING_HEXAGONS_WARP:
      _flash.reset();
      _flash.setDurationMultiplier(10);
      _flash.setPercentFlashing(60);
      break;
    default:
      break;
    }
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    _flash.setPercentBrightness(percentBrightness);
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case FLASHING_HEXAGONS:
    case FLASHING_HEXAGONS_WARP:
      _flash.show();
      break;
    default:
      break;
    }
  }
};
