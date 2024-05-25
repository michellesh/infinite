struct Action {
  unsigned long timestamp;
  uint8_t trackNumber;
  msg actionData;

  bool setTrack;
  bool setPattern;
  bool setPalette;
  bool setColorMode;
  bool setSpeed;
  bool setDensity;
  bool setWidth;
  bool setOverlaySpeed;
  bool setOverlayDensity;
  bool setOverlayWidth;
  bool setReverse;
  bool setBPM;

  Action track(uint8_t x) {
    trackNumber = x;
    setTrack = true;
    return *this;
  }

  Action pattern(int x) {
    actionData.pattern = x;
    setPattern = true;
    return *this;
  }

  Action palette(int x) {
    actionData.palette = x;
    setPalette = true;
    return *this;
  }

  Action colorMode(int x) {
    actionData.colorMode = x;
    setColorMode = true;
    return *this;
  }

  Action speed(int x) {
    actionData.speed = x;
    setSpeed = true;
    return *this;
  }

  Action density(int x) {
    actionData.density = x;
    setDensity = true;
    return *this;
  }

  Action width(int x) {
    actionData.width = x;
    setWidth = true;
    return *this;
  }

  Action overlaySpeed(int x) {
    actionData.overlaySpeed = x;
    setOverlaySpeed = true;
    return *this;
  }

  Action overlayDensity(int x) {
    actionData.overlayDensity = x;
    setOverlayDensity = true;
    return *this;
  }

  Action overlayWidth(int x) {
    actionData.overlayWidth = x;
    setOverlayWidth = true;
    return *this;
  }

  Action reverse(int x) {
    actionData.reverse = x;
    setReverse = true;
    return *this;
  }

  Action bpm(int x) {
    actionData.bpm = x;
    setBPM = true;
    return *this;
  }

  void commitData() {
    if (setPattern) {
      data.pattern = actionData.pattern;
    }
    if (setPalette) {
      data.palette = actionData.palette;
    }
    if (setColorMode) {
      data.colorMode = actionData.colorMode;
    }
    if (setSpeed) {
      data.speed = actionData.speed;
    }
    if (setDensity) {
      data.density = actionData.density;
    }
    if (setWidth) {
      data.width = actionData.width;
    }
    if (setOverlaySpeed) {
      data.overlaySpeed = actionData.overlaySpeed;
    }
    if (setOverlayDensity) {
      data.overlayDensity = actionData.overlayDensity;
    }
    if (setOverlayWidth) {
      data.overlayWidth = actionData.overlayWidth;
    }
    if (setReverse) {
      data.reverse = actionData.reverse;
    }
    if (setBPM) {
      data.bpm = actionData.bpm;
    }
  }
};
