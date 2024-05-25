class SolidSubPattern : public SubPattern {
private:
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

public:
  static const uint8_t SOLID = 0;
  static const uint8_t SOLID_OVERLAY = 1;
  static const uint8_t SOLID_OVERLAY_RAILGUN = 2;

  SolidSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case SOLID:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = palette.getColor(i);
      }
      break;
    case SOLID_OVERLAY:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = palette.getColor(i).nscale8(overlayBrightness[i]);
      }
      break;
    case SOLID_OVERLAY_RAILGUN:
      for (int i = 0; i < NUM_LEDS; i++) {
        int angle = ledAngle(i);
        bool bottomLeft = isBetween(angle, 30, 90, true);
        bool top = isBetween(angle, 150, 210, true);
        bool bottomRight = isBetween(angle, 270, 330, true);
        if (bottomLeft || top || bottomRight) {
          leds[i] = palette.getColor(i).nscale8(overlayBrightness[i]);
        }
      }
      break;
    default:
      break;
    }
  }
};
