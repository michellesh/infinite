int DEPTH_STEP_PER_RING = MAX_DEPTH / (NUM_RINGS - 1);

// Moving clockwise from bottom left to bottom right
int ANGLE_OF_STRAIGHT[] = {30, 90, 150, 210, 270, 330};

// Pin order doesn't match the angle order in ANGLE_OF_STRAIGHT
uint8_t SORT_STRAIGHT[] = {2, 3, 1, 4, 0, 5};

int ledDepth(int index) {
  // Straights
  if (index < NUM_LEDS_PER_STRAIGHT * NUM_STRAIGHTS) {
    return index % NUM_LEDS_PER_STRAIGHT;
  }

  // Rings
  int indexWithinRings = index - NUM_LEDS_PER_STRAIGHT * NUM_STRAIGHTS;
  int ringNumber = floor(indexWithinRings / NUM_LEDS_PER_RING);
  return DEPTH_STEP_PER_RING * ringNumber;
}

int ledAngle(int index) {
  // Straights
  if (index < NUM_LEDS_PER_STRAIGHT * NUM_STRAIGHTS) {
    int straightNumber = floor(index / NUM_LEDS_PER_STRAIGHT);
    return ANGLE_OF_STRAIGHT[SORT_STRAIGHT[straightNumber]];
  }

  // Rings
  int indexWithinRings = index - NUM_LEDS_PER_STRAIGHT * NUM_STRAIGHTS;
  int indexOnRing = indexWithinRings % NUM_LEDS_PER_RING;
  return map(indexOnRing, 0, NUM_LEDS_PER_RING, 330, 30);
}
