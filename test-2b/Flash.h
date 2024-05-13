class Flash : public Pattern {
private:
  Timer _flashToggleTimer = {FLASH_DURATION.DFLT};
  bool _flashing = true;
  float _durationMultiplier = 1;

  long _getFlashDuration() {
    long duration = map(speed, 10, 1, FLASH_DURATION.MIN, FLASH_DURATION.MAX);
    return duration * _durationMultiplier;
  }

  long _getDwellDuration() {
    long duration = map(speed, 10, 1, DWELL_DURATION.MIN, DWELL_DURATION.MAX);
    return duration * _durationMultiplier;
  }

public:
  static constexpr Range FLASH_DURATION = {5, 100, 50};  // milliseconds
  static constexpr Range DWELL_DURATION = {5, 500, 100}; // milliseconds

  void setDurationMultiplier(float durationMultiplier) {
    _durationMultiplier = durationMultiplier;
  }

  void show() {
    static uint8_t ringNum = NUM_RINGS - 1;
    if (_flashing) {
      for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
        rings[ringNum].leds[i] = palette.getColor(rings[ringNum].offset + i);
      }
    }
    if (_flashToggleTimer.complete()) {
      if (_flashing) {
        ringNum = ringNum == 0 ? NUM_RINGS - 1 : ringNum - 1;
      }
      _flashing = !_flashing;
      _flashToggleTimer.totalCycleTime =
          _flashing ? _getFlashDuration() : _getDwellDuration();
      _flashToggleTimer.reset();
    }
  }
};
