// clang-format off
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>

#include "utils.h"
#include "Range.h"

#include "Pattern.h"
#include "SubPattern.h"
// clang-format on

#define BRIGHTNESS 255

#define PIN_1 25
#define PIN_2 26
#define PIN_3 33
#define PIN_4 27
#define PIN_5 32
#define PIN_6 14
#define PIN_7 12
#define PIN_8 13
#define PIN_9 19
#define PIN_10 18
#define PIN_11 5

#define NUM_RINGS 10
#define NUM_STRAIGHTS 6
#define NUM_LEDS_PER_STRAIGHT 144
#define MAX_HEIGHT NUM_LEDS_PER_STRAIGHT
#define NUM_LEDS_PER_RING 144
#define NUM_LEDS                                                               \
  NUM_RINGS *NUM_LEDS_PER_RING + NUM_STRAIGHTS *NUM_LEDS_PER_STRAIGHT

CRGB leds[NUM_LEDS];
int ledHeight[NUM_LEDS];
int ledAngle[NUM_LEDS];

struct Set {
  CRGB *leds;
  int *ledAngle;
  int *ledHeight;
};

Set rings[NUM_RINGS];
Set straights[NUM_STRAIGHTS];

// clang-format off
#include "Spiral.h"
#include "SpiralSubPattern.h"
#include "Twinkle.h"
#include "TwinkleSubPattern.h"
// clang-format on

SpiralSubPattern basicSpirals(SpiralSubPattern::BASIC_SPIRALS);
TwinkleSubPattern twinkle(TwinkleSubPattern::MEDIUM_DENSITY);

void setup() {
  Serial.begin(9600);
  delay(500);

  int startIndex = 0;
  FastLED.addLeds<WS2813, PIN_1, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_2, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_3, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_4, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_5, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_6, GRB>(leds, startIndex, NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED.addLeds<WS2813, PIN_7, GRB>(leds, startIndex, NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED.addLeds<WS2813, PIN_8, GRB>(leds, startIndex, NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED.addLeds<WS2813, PIN_9, GRB>(leds, startIndex, NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED.addLeds<WS2813, PIN_10, GRB>(leds, startIndex, NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED.addLeds<WS2813, PIN_11, GRB>(leds, startIndex, NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);

  int offset = 0;
  for (int i = 0; i < NUM_STRAIGHTS; i++) {
    Set straight = {&leds[offset], &ledAngle[offset], &ledHeight[offset]};
    straights[i] = straight;
    offset += NUM_LEDS_PER_STRAIGHT;
  }
  for (int i = 0; i < NUM_RINGS; i++) {
    Set ring = {&leds[offset], &ledAngle[offset], &ledHeight[offset]};
    rings[i] = ring;
    offset += NUM_LEDS_PER_RING;
  }

  // setup heights and angles for straights.
  int angles[] = {150, 210, 90, 270, 30, 330};
  for (int i = 0; i < NUM_STRAIGHTS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRAIGHT; j++) {
      // the height is the index of the LED on that straight
      straights[i].ledHeight[j] = j;
      straights[i].ledAngle[j] = angles[i];
    }
  }

  // setup heights and angles for rings
  int heightStepPerRing = MAX_HEIGHT / (NUM_RINGS - 1);
  for (int i = 0; i < NUM_RINGS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_RING; j++) {
      rings[i].ledHeight[j] = heightStepPerRing * i;
      rings[i].ledAngle[j] = map(j, 0, NUM_LEDS_PER_RING, 330, 30);
    }
  }
}

void loop() {
  FastLED.clear();

  //basicSpirals.show();
  twinkle.show();

  FastLED.show();
}

void testHeights() {
  static int height = 0;
  int speed = 1;
  int width = 10;

  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(ledHeight[i] - height);
    if (dist < width) {
      leds[i] = CRGB::Blue;
    }
  }

  height = (height + speed + MAX_HEIGHT) % MAX_HEIGHT;
}

void testAngles() {
  static int angle = 0;
  int speed = 1;
  int width = 10;

  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(ledAngle[i] - angle);
    if (dist < width) {
      leds[i] = CRGB::Blue;
    }
  }

  angle = (angle + speed + 360) % 360;
}
