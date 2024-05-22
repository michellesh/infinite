struct Action {
  unsigned long timestamp;
  uint8_t trackNumber;
  msg actionData;

  bool setSpeed;
  bool setPattern;
  bool setTrack;

  Action track(uint8_t t) {
    trackNumber = t;
    setTrack = true;
    return *this;
  }

  Action pattern(int p) {
    actionData.pattern = p;
    setPattern = true;
    return *this;
  }

  Action speed(int s) {
    actionData.speed = s;
    setSpeed = true;
    return *this;
  }

  void commitData() {
    if (setPattern) {
      data.pattern = actionData.pattern;
    }
    if (setSpeed) {
      data.speed = actionData.speed;
    }
  }
};
