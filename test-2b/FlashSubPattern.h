class FlashSubPattern : public SubPattern {
private:
  Flash _flash;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

public:
  static const uint8_t FLASHING_HEXAGONS = 0;
  static const uint8_t FLASHING_HEXAGONS_WARP = 1;

  FlashSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case FLASHING_HEXAGONS_WARP:
      _flash.setDurationMultiplier(0.1);
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
