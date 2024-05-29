void waveOverlay() {
  int width = map(overlayWidth, 1, 10, MAX_DEPTH / 3, MAX_DEPTH * 2);
  int offset = map(overlayDensity, 1, 10, MAX_DEPTH, 0) + width / 2;
  int startPosition = reverse ? -offset : MAX_DEPTH + offset;
  int endPosition = reverse ? MAX_DEPTH + offset : -offset;
  int position = mapBeat(startPosition, endPosition);
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

uint8_t sinWaveGlobalBrightness() {
  int sinBrightness = mapBeat(0, 255);
  static bool alternate = true;
  static int prevSinBrightness = sinBrightness;
  if (sinBrightness < prevSinBrightness) {
    alternate = !alternate;
  }
  prevSinBrightness = sinBrightness;
  return alternate ? sinBrightness : 255 - sinBrightness;
}

uint8_t strobeGlobalBrightness() { return mapBeat(0, 100) < 50 ? 255 : 0; }
