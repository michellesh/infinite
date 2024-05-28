class SubPattern {
 public:
  virtual uint8_t getPercentBrightness() {}

  virtual void setPercentBrightness(uint8_t percentBrightness) {}

  virtual void setActivePattern(uint8_t activeSubPattern) {}

  virtual void setup() {}

  virtual void show() {}
};
