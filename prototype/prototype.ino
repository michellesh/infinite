// clang-format off
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Time.h>

#include "Range.h"
#include "Timer.h"
#include "utils.h"
#include "colors.h"

#include "Pattern.h"
#include "SubPattern.h"
#include "InfiniteShared.h"
#include "mode.h"
// clang-format on

#define BRIGHTNESS 255

#if MODE == XX_RECEIVER_MODE
#define PIN_STRAIGHT_1 25 // M1
#define PIN_STRAIGHT_2 26 // M1
#define PIN_STRAIGHT_3 4  // M2
#define PIN_STRAIGHT_4 2  // M2
#define PIN_STRAIGHT_5 4  // M3
#define PIN_STRAIGHT_6 2  // M3
#define PIN_RINGS_1_2 4   // M4
#define PIN_RINGS_3_4 4   // M5
#define PIN_RINGS_5_6 4   // M6
#define PIN_RINGS_7_8 4   // M7
#define PIN_RINGS_9_10 4  // M8
#else
#define PIN_STRAIGHT_1 25
#define PIN_STRAIGHT_2 26
#define PIN_STRAIGHT_3 33
#define PIN_STRAIGHT_4 27
#define PIN_STRAIGHT_5 32
#define PIN_STRAIGHT_6 14
#define PIN_RINGS_1_2 12
#define PIN_RINGS_3_4 13
#define PIN_RINGS_5_6 19
#define PIN_RINGS_7_8 18
#define PIN_RINGS_9_10 5
#endif

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
CRGB ledsBlank[NUM_LEDS];
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
#define PATTERN_SOLID_OVERLAY_RAILGUN 1
#define PATTERN_TWINKLE 2
#define PATTERN_RANDOM_FADING_SEGMENTS 3
#define PATTERN_RANDOM_FLASHING_SEGMENTS 4
#define PATTERN_TWINKLE_OVERLAY 5
#define PATTERN_SINGLE_SPIRAL 6
#define PATTERN_DOUBLE_SPIRAL 7
#define PATTERN_ROTATING_PONG 8
#define PATTERN_LASERS 9
#define PATTERN_LASERS_ALL_AT_ONCE 10
#define PATTERN_RAINFALL 11
#define PATTERN_BASKET_WEAVING 12
#define PATTERN_COMET_TRAILS 13
#define PATTERN_ROTATING_HEXAGONS 14
#define PATTERN_COUNTER_ROTATING_HEXAGONS 15
#define PATTERN_VARIABLE_SPEED_ROTATION 16
#define PATTERN_FLASHING_HEXAGONS 17
#define PATTERN_FLASHING_HEXAGONS_WARP 18
#define NUM_PATTERNS 19
int activePattern = PATTERN_TWINKLE;
int speed = DEFAULT_SPEED;
int density = DEFAULT_DENSITY;
int width = DEFAULT_WIDTH;
int overlaySpeed = DEFAULT_OVERLAYSPEED;
int overlayWidth = DEFAULT_OVERLAYWIDTH;
int overlayDensity = DEFAULT_OVERLAYDENSITY;
bool reverse = DEFAULT_REVERSE;

msg data;

// clang-format off
#include "twinkleUtils.h"
#include "ledDepth-ledAngle.h"
#include "overlay.h"

#include "Palette.h"
Palette palette;

//#include "web_server.h"
#include "handleAction.h"

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

SolidSubPattern solidOverlay(SolidSubPattern::SOLID_OVERLAY);
SolidSubPattern solidOverlayRailgun(SolidSubPattern::SOLID_OVERLAY_RAILGUN);
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
  &solidOverlayRailgun,
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

ESP32Time rtc;

Timer actionTimer;
bool actionQueued = false;

void setup() {
  Serial.begin(9600);
  delay(500);
  randomSeed(analogRead(0));

  rtc.setTime(30, 24, 15, 17, 1, 2024); // 17th Jan 2024 15:24:30

#if MODE != SINGLE_BOARD_MODE
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
#endif

#if MODE == XX_RECEIVER_MODE

  if (WiFi.macAddress() == receiverM1) {

    int startIndex = 0;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_1, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

    startIndex += NUM_LEDS_PER_STRAIGHT;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_2, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM2) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 2;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_3, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

    startIndex += NUM_LEDS_PER_STRAIGHT;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_4, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM3) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 4;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_5, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

    startIndex += NUM_LEDS_PER_STRAIGHT;
    FastLED
        .addLeds<WS2813, PIN_STRAIGHT_6, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_STRAIGHT)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM4) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 6;
    FastLED
        .addLeds<WS2813, PIN_RINGS_1_2, GRB>(leds, startIndex,
                                             NUM_LEDS_PER_RING * 2)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM5) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 6 + NUM_LEDS_PER_RING * 2;
    FastLED
        .addLeds<WS2813, PIN_RINGS_3_4, GRB>(leds, startIndex,
                                             NUM_LEDS_PER_RING * 2)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM6) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 6 + NUM_LEDS_PER_RING * 4;
    FastLED
        .addLeds<WS2813, PIN_RINGS_5_6, GRB>(leds, startIndex,
                                             NUM_LEDS_PER_RING * 2)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM7) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 6 + NUM_LEDS_PER_RING * 6;
    FastLED
        .addLeds<WS2813, PIN_RINGS_7_8, GRB>(leds, startIndex,
                                             NUM_LEDS_PER_RING * 2)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);

  } else if (WiFi.macAddress() == receiverM8) {

    int startIndex = NUM_LEDS_PER_STRAIGHT * 6 + NUM_LEDS_PER_RING * 8;
    FastLED
        .addLeds<WS2813, PIN_RINGS_9_10, GRB>(leds, startIndex,
                                              NUM_LEDS_PER_RING * 2)
        .setCorrection(TypicalLEDStrip)
        .setDither(BRIGHTNESS < 255);
  }

#else // SINGLE_BOARD_MODE

  int startIndex = 0;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_1, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_2, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_3, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_4, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_5, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_STRAIGHT_6, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_STRAIGHT)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_STRAIGHT;
  FastLED
      .addLeds<WS2813, PIN_RINGS_1_2, GRB>(leds, startIndex,
                                           NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED
      .addLeds<WS2813, PIN_RINGS_3_4, GRB>(leds, startIndex,
                                           NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED
      .addLeds<WS2813, PIN_RINGS_5_6, GRB>(leds, startIndex,
                                           NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED
      .addLeds<WS2813, PIN_RINGS_7_8, GRB>(leds, startIndex,
                                           NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  startIndex += NUM_LEDS_PER_RING * 2;
  FastLED
      .addLeds<WS2813, PIN_RINGS_9_10, GRB>(leds, startIndex,
                                            NUM_LEDS_PER_RING * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);

#endif

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

  // setupWebServer();

  for (int i = 0; i < NUM_PATTERNS; i++) {
    activePatterns[i]->setup();
  }
}

void loop() {
  FastLED.clear();
  palette.cycle();

  EVERY_N_SECONDS(1) {
    //Serial.print("Local IP address: ");
    //Serial.println(WiFi.localIP());
  }

  if (actionQueued && actionTimer.complete()) {
    handleAction(data.action, data.value);
    actionQueued = false;
  }

  if (activePattern == PATTERN_TWINKLE_OVERLAY ||
      activePattern == PATTERN_SOLID_OVERLAY ||
      activePattern == PATTERN_SOLID_OVERLAY_RAILGUN) {
    waveOverlay();
  }

  static int prevActivePattern = -1;
  if (activePattern != prevActivePattern) {
    activePatterns[activePattern]->setup();
    prevActivePattern = activePattern;
  }

  activePatterns[activePattern]->show();

  static int prevSecond = -1;
  static unsigned long prevTicks = ticks;
  int currentSecond = rtc.getSecond();
  if (prevSecond != currentSecond) {
    Serial.print("Frames in last second: ");
    Serial.println(ticks - prevTicks);
    prevSecond = currentSecond;
    prevTicks = ticks;
  }

  FastLED.setMaxRefreshRate(30);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  ticks++;
}

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  Serial.print("action: ");
  Serial.println(data.action);
  Serial.print("value: ");
  Serial.println(data.value);
  Serial.print("delay: ");
  Serial.println(data.delay);

  actionQueued = true;
  actionTimer.totalCycleTime = data.delay;
  actionTimer.reset();
}
