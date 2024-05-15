class SolidSubPattern : public SubPattern {
private:
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

public:
  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
  }

  virtual void show() {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = palette.getColor(i).nscale8(overlayBrightness[i]);
    }
  }
};
