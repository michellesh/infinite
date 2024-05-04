#define TWINKLE_DENSITY 6 // 0 (NONE lit) to 8 (ALL lit at once)
#define TWINKLE_SPEED 3 // 0-8

CRGB backgroundColor = CRGB::Black;

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
void twinkle() {

  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;

  uint32_t clock32 = millis();

  for (int i = 0; i < NUM_LEDS; i++) {
    int r = map(i, 0, NUM_LEDS, 0, 255);
    CRGB color = CRGB(r, 100, 100);
    leds[i] = getTwinkleColor(PRNG16, clock32, color);
    //leds2[i] = getTwinkleColor(PRNG16, clock32, color);
  }
}

CRGB getTwinkleColor(uint16_t &PRNG16, uint32_t clock32, CRGB color) {
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
  CRGB c = applyTwinkleBrightness(myclock30, myunique8,
                                  TWINKLE_SPEED, color);

  uint8_t cbright = c.getAverageLight();
  int16_t deltabright = cbright;
  if (deltabright >= 32) {
    // If the new pixel is significantly brighter than the background color,
    // use the new color.
    return c;
  } else if (deltabright > 0) {
    // If the new pixel is just slightly brighter than the background color,
    // mix a blend of the new color and the background color
    return blend(backgroundColor, c, deltabright * 8);
  } else {
    // if the new pixel is not at all brighter than the background color,
    // just use the background color.
    return backgroundColor;
  }
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB applyTwinkleBrightness(uint32_t ms, uint8_t salt, uint8_t speed,
                            CRGB color) {
  uint16_t ticks = ms >> (8 - speed);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  return bright > 0 ? color.nscale8(bright) : CRGB::Black;
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}
