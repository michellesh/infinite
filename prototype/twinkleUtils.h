#define TWINKLE_DENSITY 6 // 0 (NONE lit) to 8 (ALL lit at once)
#define TWINKLE_SPEED 5   // 0-8

CRGB backgroundColor = CRGB::Black;

uint8_t getTwinkleBrightness(uint16_t &PRNG16, uint32_t clock32, uint8_t speed) {
  PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
  uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
  PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
  // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to
  // 23/8ths)
  uint8_t myspeedmultiplierQ5_3 =
      ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
  uint32_t myclock30 =
      (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
  uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

  // We now have the adjusted 'clock' for this pixel, now we call
  // the function that computes what color the pixel should be based
  // on the "brightness = f( time )" idea.
  uint16_t ticks = myclock30 >> (8 - speed);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + myunique8;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  return bright;
}

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
void twinkleSome(uint8_t *leds, int numLEDs) {

  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;

  uint32_t clock32 = millis();

  for (int i = 0; i < numLEDs; i++) {
    uint8_t brightness = getTwinkleBrightness(PRNG16, clock32, TWINKLE_SPEED);
    //leds[i].nscale8(brightness);
    leds[i] = brightness;
  }
}
