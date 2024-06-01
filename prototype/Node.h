struct Node {
  int straightNum;
  int straightPosition;
  int ringNum;
  int ringPosition;
};

int ringPositionOnStraight(int straightNum) {
  return NUM_LEDS_PER_RING - (straightNum * RING_SEGMENT_LENGTH);
}

int straightPositionOnRing(int ringNum) {
  return NUM_LEDS_PER_STRAIGHT / (NUM_RINGS - 1) * ringNum;
}
