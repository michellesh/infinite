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
#define DEPTH_SEGMENT_LENGTH MAX_DEPTH / (NUM_RINGS - 1)
#define ANGLE_SEGMENT_LENGTH 360 / NUM_STRAIGHTS

CRGB leds[NUM_LEDS];
int ledDepth[NUM_LEDS];
int ledAngle[NUM_LEDS];

struct Path {
  CRGB *leds;
  int *ledAngle;
  int *ledDepth;
  int length;
  int offset;
};

Path rings[NUM_RINGS];
Path straights[NUM_STRAIGHTS];

// globals controlled by web server
int activePattern = 2;
#define PATTERN_TWINKLE 0
#define PATTERN_BASIC_SPIRAL 1
#define PATTERN_ROTATING_PONG 2
#define NUM_PATTERNS 3
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
#include "Line.h"
#include "LineSubPattern.h"
// clang-format on

TwinkleSubPattern twinkle(TwinkleSubPattern::MEDIUM_DENSITY);
SpiralSubPattern basicSpirals(SpiralSubPattern::BASIC_SPIRALS);
LineSubPattern rotatingPong(LineSubPattern::ROTATING_PONG);

// clang-format off
SubPattern *activePatterns[] = {
  &twinkle,
  &basicSpirals,
  &rotatingPong
};
// clang-format on

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
  uint8_t sortStraight[] = {2, 3, 1, 4, 0, 5};
  for (int i = 0; i < NUM_STRAIGHTS; i++) {
    Path straight = {&leds[offset], &ledAngle[offset], &ledDepth[offset],
                     NUM_LEDS_PER_STRAIGHT, offset};
    straights[sortStraight[i]] = straight;
    offset += NUM_LEDS_PER_STRAIGHT;
  }
  for (int i = 0; i < NUM_RINGS; i++) {
    Path ring = {&leds[offset], &ledAngle[offset], &ledDepth[offset],
                 NUM_LEDS_PER_RING, offset};
    rings[i] = ring;
    offset += NUM_LEDS_PER_RING;
  }

  // setup depths and angles for straights.
  int angles[] = {30, 90, 150, 210, 270, 330};
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

  for (int i = 0; i < NUM_PATTERNS; i++) {
    activePatterns[i]->setup();
  }
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

  activePatterns[activePattern]->show();

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
