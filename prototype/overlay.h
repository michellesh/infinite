void waveOverlay() {
  int width = map(overlayWidth, 1, 10, MAX_DEPTH / 3, MAX_DEPTH * 2);
  int waveLength = map(overlaySpeed, 1, 10, 100, 10);
  int offset = map(overlayDensity, 1, 10, MAX_DEPTH, 0) + width / 2;
  int startPosition = reverse ? -offset : MAX_DEPTH + offset;
  int endPosition = reverse ? MAX_DEPTH + offset : -offset;
  int position = sawtooth(startPosition, endPosition, waveLength);
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
