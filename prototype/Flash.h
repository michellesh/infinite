class Flash : public Pattern {
private:
  bool _flashing = true;
  float _durationMultiplier = 1;
  int _percentFlashing = 30;

public:
  void reset() {
    _flashing = true;
    _durationMultiplier = 1;
    _percentFlashing = 30;
  }

  void setDurationMultiplier(float durationMultiplier) {
    _durationMultiplier = durationMultiplier;
  }

  void setPercentFlashing(float percentFlashing) {
    _percentFlashing = percentFlashing;
  }

  void show() {
    static int ringNum = reverse ? 0 : NUM_RINGS - 1;
    int percent = mapBeat(0, 100, _durationMultiplier);
    static int prevPercent = percent;
    if (percent < prevPercent) {
      ringNum = (ringNum + (reverse ? 1 : -1) + NUM_RINGS) % NUM_RINGS;
    }
    if (percent < _percentFlashing) {
      for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
        rings[ringNum].leds[i] = palette.getColor(rings[ringNum].offset + i);
      }
    }
    prevPercent = percent;
  }
};
