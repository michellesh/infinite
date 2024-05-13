class Pattern {
 private:
  uint8_t _percentBrightness = 0;  // percent brightness of the whole pattern

 public:
  uint8_t getPercentBrightness() { return _percentBrightness; }

  void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
  }
};
