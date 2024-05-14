void waveOverlay() {
  int width = MAX_DEPTH / 3;
  int position = sawtooth(MAX_DEPTH + width / 2, -width / 2, 20);
  for (int i = 0; i < NUM_LEDS; i++) {
    int depth = ledDepth(i);
    int distFromPosition = abs(depth - position);
    if (distFromPosition < width / 2) {
      overlayBrightness[i] = map(distFromPosition, 0, width / 2, 255, 0);
    } else {
      overlayBrightness[i] = 0;
    }
  }
}
