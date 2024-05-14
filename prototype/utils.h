float mapf(float value, float inMin, float inMax, float outMin, float outMax) {
  float percentage = (value - inMin) / (inMax - inMin);
  return outMin + (outMax - outMin) * percentage;
}

bool isBetween(int16_t value, int16_t min, int16_t max,
               bool inclusive = false) {
  return inclusive ? value >= min && value <= max : value > min && value < max;
}

struct XY {
  uint8_t x;
  uint8_t y;
};

// creates a "fade shape" based on 2 or 3 pivot points.
// assumes p1.x < p2.x < p3.x
uint8_t fadeShape(uint8_t x, XY p1, XY p2, XY p3) {
  // clang-format off
  return x < p1.x   ? map(x, 0, p1.x, 0, p1.y)
         : x < p2.x ? map(x, p1.x, p2.x, p1.y, p2.y)
         : x < p3.x ? map(x, p2.x, p3.x, p2.y, p3.y)
                    : map(x, p3.x, 255, p3.y, 0);
  // clang-format on
}

uint8_t fadeShape(uint8_t x, XY p1, XY p2 = {127, 255}) {
  return fadeShape(x, p1, p2, {255 - p1.x, p1.y});
}

uint8_t addFadeShape(uint8_t x) { return fadeShape(x, {60, 20}); }

uint8_t basicFade(uint8_t x, uint8_t max = 255) {
  return x < 127 ? map(x, 0, 127, 0, max) : map(x, 127, 255, max, 0);
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//
uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

unsigned long ticks = 0;

float sinwave(float minValue, float maxValue, unsigned long waveLength = 50,
              unsigned long waveLengthOffset = 0) {
  return mapf(sin((float)(ticks + waveLengthOffset) * PI / waveLength), -1, 1,
              minValue, maxValue);
}

float sawtooth(float minValue, float maxValue, unsigned long waveLength = 50,
               unsigned long waveLengthOffset = 0) {
  float t = (float)(ticks + waveLengthOffset) / waveLength;
  return mapf(t - floor(t), 0, 1, minValue, maxValue);
}
