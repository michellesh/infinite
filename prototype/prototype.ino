// clang-format off
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Range.h"
#include "Timer.h"
#include "utils.h"
#include "colors.h"

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
#define NUM_LEDS_PER_RING 144
#define NUM_LEDS                                                               \
  NUM_RINGS *NUM_LEDS_PER_RING + NUM_STRAIGHTS *NUM_LEDS_PER_STRAIGHT

#define MAX_DEPTH NUM_LEDS_PER_STRAIGHT

CRGB leds[NUM_LEDS];
int ledDepth[NUM_LEDS];
int ledAngle[NUM_LEDS];

struct Set {
  CRGB *leds;
  int *ledAngle;
  int *ledDepth;
};

Set rings[NUM_RINGS];
Set straights[NUM_STRAIGHTS];

// globals controlled by web server
int activePattern = 0;
#define PATTERN_TWINKLE 0
#define PATTERN_BASIC_SPIRAL 1
#define NUM_PATTERNS 2
int speed = 3;
bool autoCyclePalettes = true;

// clang-format off
#include "Palette.h"
Palette palette;

#include "web_server.h"

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
    Set straight = {&leds[offset], &ledAngle[offset], &ledDepth[offset]};
    straights[i] = straight;
    offset += NUM_LEDS_PER_STRAIGHT;
  }
  for (int i = 0; i < NUM_RINGS; i++) {
    Set ring = {&leds[offset], &ledAngle[offset], &ledDepth[offset]};
    rings[i] = ring;
    offset += NUM_LEDS_PER_RING;
  }

  // setup depths and angles for straights.
  int angles[] = {150, 210, 90, 270, 30, 330};
  for (int i = 0; i < NUM_STRAIGHTS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRAIGHT; j++) {
      // the depth is the index of the LED on that straight
      straights[i].ledDepth[j] = j;
      straights[i].ledAngle[j] = angles[i];
    }
  }

  // setup depths and angles for rings
  int depthStepPerRing = MAX_DEPTH / (NUM_RINGS - 1);
  for (int i = 0; i < NUM_RINGS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_RING; j++) {
      rings[i].ledDepth[j] = depthStepPerRing * i;
      rings[i].ledAngle[j] = map(j, 0, NUM_LEDS_PER_RING, 330, 30);
    }
  }

  setupWebServer();
}

void loop() {
  FastLED.clear();
  if (autoCyclePalettes) {
    palette.cycle();
  }

  EVERY_N_SECONDS(1) {
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
  }

  switch (activePattern) {
    case PATTERN_TWINKLE:
      twinkle.show();
      break;
    case PATTERN_BASIC_SPIRAL:
      basicSpirals.show();
      break;
    default:
      break;
  }

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}

void testDepths() {
  static int depth = 0;
  int speed = 1;
  int width = 10;

  for (int i = 0; i < NUM_LEDS; i++) {
    int dist = abs(ledDepth[i] - depth);
    if (dist < width) {
      leds[i] = CRGB::Blue;
    }
  }

  depth = (depth + speed + MAX_DEPTH) % MAX_DEPTH;
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
