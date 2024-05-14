void waveOverlay() {
  int width = map(overlayWidth, 1, 10, MAX_DEPTH / 3, MAX_DEPTH);
  int waveLength = map(overlaySpeed, 1, 10, 100, 10);
  int from = reverse ? -width / 2 : MAX_DEPTH + width / 2;
  int to = reverse ? MAX_DEPTH + width / 2 : -width / 2;
  int position = sawtooth(from, to, waveLength);
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
