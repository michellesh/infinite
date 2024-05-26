void handleAction() {
  if (data.pattern != activePattern) {
    activePattern = data.pattern;
    ticks = 0;
    // every time we start a new pattern, start at time 0
    // when millis() gets really large, mapping the current progress of a beat
    // using % results in more variation,
    loopMillis = 0;
  }
  if (data.palette != palette.getPalette()) {
    palette.setPalette(data.palette);
  }
  if (data.colorMode != palette.getColorMode()) {
    palette.setColorMode(data.colorMode);
  }

  speed = data.speed;
  density = data.density;
  width = data.width;
  overlaySpeed = data.overlaySpeed;
  overlayDensity = data.overlayDensity;
  overlayWidth = data.overlayWidth;
  reverse = data.reverse;
  bpm = data.bpm;
}

void setData(int action, int value = 0) {
  switch (action) {
  case ACTION_SET_PATTERN:
    data.pattern = value;
    break;
  case ACTION_SET_PALETTE:
    data.palette = value;
    break;
  case ACTION_SET_COLOR_MODE:
    data.colorMode = value;
    break;
  case ACTION_SET_SPEED:
    data.speed = value;
    break;
  case ACTION_SET_DENSITY:
    data.density = value;
    break;
  case ACTION_SET_WIDTH:
    data.width = value;
    break;
  case ACTION_SET_OVERLAY_SPEED:
    data.overlaySpeed = value;
    break;
  case ACTION_SET_OVERLAY_WIDTH:
    data.overlayWidth = value;
    break;
  case ACTION_SET_OVERLAY_DENSITY:
    data.overlayDensity = value;
    break;
  case ACTION_TOGGLE_REVERSE:
    data.reverse = !data.reverse;
    break;
  case ACTION_SET_BPM:
    data.bpm = value;
    break;
  default:
    break;
  }
}
