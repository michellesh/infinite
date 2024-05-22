void handleAction() {
  if (data.pattern != activePattern) {
    ticks = 0;
    activePattern = data.pattern;
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
}
