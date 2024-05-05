#include <FastLED.h>

#define LED_PIN 12
#define LED_PIN_2 14
#define LED_PIN_3 27
#define LED_PIN_4 26
#define LED_PIN_5 25
#define LED_PIN_6 33
#define LED_PIN_7 32
#define LED_PIN_8 2
#define NUM_ROLLS 4
#define NUM_LEDS_PER_ROLL 300
#define NUM_LEDS NUM_LEDS_PER_ROLL *NUM_ROLLS
#define NUM_PINS 7
#define BRIGHTNESS 100


CRGB leds[NUM_PINS][NUM_LEDS];

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<WS2813, LED_PIN, GRB>(leds[0], NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, LED_PIN_2, GRB>(leds[1], NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  //FastLED.addLeds<WS2813, LED_PIN_3, GRB>(leds[2], NUM_LEDS)
  //    .setCorrection(TypicalLEDStrip)
  //    .setDither(BRIGHTNESS < 255);
  //FastLED.addLeds<WS2813, LED_PIN_4, GRB>(leds[3], NUM_LEDS)
  //    .setCorrection(TypicalLEDStrip)
  //    .setDither(BRIGHTNESS < 255);
  //FastLED.addLeds<WS2813, LED_PIN_5, GRB>(leds[4], NUM_LEDS)
  //    .setCorrection(TypicalLEDStrip)
  //    .setDither(BRIGHTNESS < 255);
  //FastLED.addLeds<WS2813, LED_PIN_6, GRB>(leds[5], NUM_LEDS)
  //    .setCorrection(TypicalLEDStrip)
  //    .setDither(BRIGHTNESS < 255);
  //FastLED.addLeds<WS2813, LED_PIN_7, GRB>(leds[6], NUM_LEDS)
  //    .setCorrection(TypicalLEDStrip)
  //    .setDither(BRIGHTNESS < 255);
}

void loop() {
  EVERY_N_SECONDS(1) {
    Serial.print("NUM_LEDS: ");
    Serial.println(NUM_LEDS);
  }

  twinkle();
  FastLED.setBrightness(100);
  FastLED.show();
}

/*
// This function is taken from the FastLED example Pride2015
void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}

void juggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);

  uint8_t dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void sinBeat() {
  uint8_t sinBeat = beatsin8(1, 0, NUM_LEDS - 1, 0, 0);
  uint8_t sinBeat2 = beatsin8(1, 0, NUM_LEDS - 1, 0, 85);
  uint8_t sinBeat3 = beatsin8(1, 0, NUM_LEDS - 1, 0, 170);

  leds[sinBeat] = CRGB::Blue;
  leds[sinBeat2] = CRGB::Red;
  leds[sinBeat3] = CRGB::Purple;

  fadeToBlackBy(leds, NUM_LEDS, 10);
}
*/
