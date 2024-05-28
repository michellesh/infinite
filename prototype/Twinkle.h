class Twinkle : public Pattern {
private:
  float _speedMultiplier = 1;
  float _densityMultiplier = 1;
  float _widthMultiplier = 1;
  bool _withOverlay = false;

  int _getSpeed() { return speed * _speedMultiplier; }

  int _getDensity() { return density * _densityMultiplier; }

  int _getWidth() { return width * _widthMultiplier; }

  uint8_t _getBrightness(uint16_t &PRNG16, uint32_t clock32) {
    // Use pseudo random number generator to get values for the clock speed
    // adjustment and clock offset of this pixel
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16 = PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from
    // 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =
        ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 =
        (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    uint16_t ticks = myclock30 >> (8 - min(8, _getSpeed()));
    uint8_t fastcycle8 = ticks;
    uint16_t slowcycle16 = (ticks >> 8) + myunique8;
    slowcycle16 += sin8(slowcycle16);
    slowcycle16 = (slowcycle16 * 2053) + 1384;
    uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

    return ((slowcycle8 & 0x0E) / 2) < _getDensity()
               ? attackDecayWave8(fastcycle8)
               : 0;
  }

public:
  void setSpeedMultiplier(float speedMultiplier) {
    _speedMultiplier = speedMultiplier;
  }

  void setDensityMultiplier(float densityMultiplier) {
    _densityMultiplier = densityMultiplier;
  }

  void setWidthMultiplier(float widthMultiplier) {
    _widthMultiplier = widthMultiplier;
  }

  void setWithOverlay(bool withOverlay) { _withOverlay = withOverlay; }

  void reset() {
    _speedMultiplier = 1;
    _densityMultiplier = 1;
    _widthMultiplier = 1;
    _withOverlay = false;
  }

  void show() {
    uint16_t PRNG16 = 11337;
    uint32_t clock32 = millis();

    int groupLength = _getWidth();
    if (groupLength > 1) {
      int currentGroup = 0;
      uint8_t brightness;
      for (int i = 0; i < NUM_LEDS; i++) {
        if (i % groupLength == 0) {
          currentGroup += groupLength;
          brightness = _getBrightness(PRNG16, clock32);
        }
        CRGB color = palette.getColor(i);
        uint8_t distInGroup =
            map(i, currentGroup, currentGroup + groupLength, 0, 255);
        uint8_t b = basicFade(distInGroup, brightness);
        if (_withOverlay) {
          b = b > overlayBrightness[i] ? b : overlayBrightness[i];
        }
        leds[i] = color.nscale8(b);
      }
    } else {
      for (int i = 0; i < NUM_LEDS; i++) {
        CRGB color = palette.getColor(i);
        uint8_t b = _getBrightness(PRNG16, clock32);
        if (_withOverlay) {
          b = b > overlayBrightness[i] ? b : overlayBrightness[i];
        }
        leds[i] = color.nscale8(b);
      }
    }
  }
};
