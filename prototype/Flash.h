class Flash : public Pattern {
private:
  float _speedMultiplier = 1;
  long _flashTime = 50;
  long _dwellTime = 100;
  Timer _flashToggleTimer = {_flashTime};
  bool _flashing = true;

  int _getSpeed() { return speed * _speedMultiplier; }

public:
  void setSpeedMultiplier(float speedMultiplier) {
    _speedMultiplier = speedMultiplier;
  }

  void setFlashTime(long flashTime) { _flashTime = flashTime; }

  void setDwellTime(long dwellTime) { _dwellTime = dwellTime; }

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
      _flashToggleTimer.totalCycleTime = _flashing ? _flashTime : _dwellTime;
      _flashToggleTimer.reset();
    }
  }
};
