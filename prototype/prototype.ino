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
uint8_t twinkleBrightness[NUM_LEDS];
uint8_t overlayBrightness[NUM_LEDS];

struct Path {
  CRGB *leds;
  int length;
  int offset;
};

Path rings[NUM_RINGS];
Path straights[NUM_STRAIGHTS];

// globals controlled by web server
#define PATTERN_SOLID_OVERLAY 0
#define PATTERN_TWINKLE 1
#define PATTERN_RANDOM_FADING_SEGMENTS 2
#define PATTERN_RANDOM_FLASHING_SEGMENTS 3
#define PATTERN_TWINKLE_OVERLAY 4
#define PATTERN_SINGLE_SPIRAL 5
#define PATTERN_DOUBLE_SPIRAL 6
#define PATTERN_ROTATING_PONG 7
#define PATTERN_LASERS 8
#define PATTERN_LASERS_ALL_AT_ONCE 9
#define PATTERN_RAINFALL 10
#define PATTERN_BASKET_WEAVING 11
#define PATTERN_COMET_TRAILS 12
#define PATTERN_ROTATING_HEXAGONS 13
#define PATTERN_COUNTER_ROTATING_HEXAGONS 14
#define PATTERN_VARIABLE_SPEED_ROTATION 15
#define PATTERN_FLASHING_HEXAGONS 16
#define PATTERN_FLASHING_HEXAGONS_WARP 17
#define NUM_PATTERNS 18
int activePattern = 0;
int speed = 3;
int overlaySpeed = 8;
int overlayWidth = 5;
int density = 4;
int width = 5;
bool reverse = false;
bool autoCyclePalettes = true;

// clang-format off
#include "twinkleUtils.h"
#include "ledDepth-ledAngle.h"
#include "overlay.h"

#include "Palette.h"
Palette palette;

#include "web_server.h"

#include "Spiral.h"
#include "SpiralSubPattern.h"
#include "Twinkle.h"
#include "TwinkleSubPattern.h"
#include "Line.h"
#include "LineSubPattern.h"
#include "Flash.h"
#include "FlashSubPattern.h"
#include "SolidSubPattern.h"
// clang-format on

SolidSubPattern solidOverlay;
TwinkleSubPattern twinkle(TwinkleSubPattern::TWINKLE);
TwinkleSubPattern
    randomFadingSegments(TwinkleSubPattern::RANDOM_FADING_SEGMENTS);
LineSubPattern randomFlashingSegments(LineSubPattern::RANDOM_FLASHING_SEGMENTS);
TwinkleSubPattern twinkleOverlay(TwinkleSubPattern::TWINKLE_OVERLAY);
SpiralSubPattern singleSpiral(SpiralSubPattern::SINGLE_SPIRAL);
SpiralSubPattern doubleSpiral(SpiralSubPattern::DOUBLE_SPIRAL);
LineSubPattern rotatingPong(LineSubPattern::ROTATING_PONG);
LineSubPattern lasers(LineSubPattern::LASERS);
LineSubPattern lasersAllAtOnce(LineSubPattern::LASERS_ALL_AT_ONCE);
LineSubPattern rainfall(LineSubPattern::RAINFALL);
LineSubPattern basketWeaving(LineSubPattern::BASKET_WEAVING);
LineSubPattern cometTrails(LineSubPattern::COMET_TRAILS);
LineSubPattern rotatingHexagons(LineSubPattern::ROTATING_HEXAGONS);
LineSubPattern
    counterRotatingHexagons(LineSubPattern::COUNTER_ROTATING_HEXAGONS);
LineSubPattern variableSpeedRotation(LineSubPattern::VARIABLE_SPEED_ROTATION);
FlashSubPattern flashingHexagons(FlashSubPattern::FLASHING_HEXAGONS);
FlashSubPattern flashingHexagonsWarp(FlashSubPattern::FLASHING_HEXAGONS_WARP);

// clang-format off
SubPattern *activePatterns[] = {
  &solidOverlay,
  &twinkle,
  &randomFadingSegments,
  &randomFlashingSegments,
  &twinkleOverlay,
  &singleSpiral,
  &doubleSpiral,
  &rotatingPong,
  &lasers,
  &lasersAllAtOnce,
  &rainfall,
  &basketWeaving,
  &cometTrails,
  &rotatingHexagons,
  &counterRotatingHexagons,
  &variableSpeedRotation,
  &flashingHexagons,
  &flashingHexagonsWarp
};
// clang-format on

void setup() {
  Serial.begin(9600);
  delay(500);
  randomSeed(analogRead(0));

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
    Path straight = {&leds[offset], NUM_LEDS_PER_STRAIGHT, offset};
    straights[sortStraight[i]] = straight;
    offset += NUM_LEDS_PER_STRAIGHT;
  }
  for (int i = 0; i < NUM_RINGS; i++) {
    Path ring = {&leds[offset], NUM_LEDS_PER_RING, offset};
    rings[i] = ring;
    offset += NUM_LEDS_PER_RING;
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

  if (activePattern == PATTERN_TWINKLE_OVERLAY ||
      activePattern == PATTERN_SOLID_OVERLAY) {
    waveOverlay();
  }

  activePatterns[activePattern]->show();

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  ticks++;
}
