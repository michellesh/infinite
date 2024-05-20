void handleAction(uint8_t action, int value = 0) {
  if (action == ACTION_SET_PATTERN) {
    activePattern = value;
  } else if (action == ACTION_SET_PALETTE) {
    palette.setPalette(value);
    autoCyclePalettes = false;
  } else if (action == ACTION_TOGGLE_AUTO_CYCLE_PALETTES) {
    autoCyclePalettes = !autoCyclePalettes;
  } else if (action == ACTION_SET_SECONDS_PER_PALETTE) {
    palette.setSecondsPerPalette(value);
  } else if (action == ACTION_SET_COLOR_MODE) {
    palette.setColorMode(value);
  } else if (action == ACTION_SET_SPEED) {
    speed = value;
  } else if (action == ACTION_SET_DENSITY) {
    density = value;
  } else if (action == ACTION_SET_WIDTH) {
    width = value;
  } else if (action == ACTION_SET_OVERLAY_SPEED) {
    overlaySpeed = value;
  } else if (action == ACTION_SET_OVERLAY_WIDTH) {
    overlayWidth = value;
  } else if (action == ACTION_SET_OVERLAY_DENSITY) {
    overlayDensity = value;
  } else if (action == ACTION_TOGGLE_REVERSE) {
    reverse = !reverse;
  }
}
