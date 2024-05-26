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
#include "InfiniteShared.h"
// clang-format on

#define BRIGHTNESS 255

#if MODE == X8_RECEIVER_MODE
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

int brightness = BRIGHTNESS;
int activePattern = DEFAULT_PATTERN;
float speed = DEFAULT_SPEED;
float density = DEFAULT_DENSITY;
float width = DEFAULT_WIDTH;
float overlaySpeed = DEFAULT_OVERLAYSPEED;
float overlayWidth = DEFAULT_OVERLAYWIDTH;
float overlayDensity = DEFAULT_OVERLAYDENSITY;
bool reverse = DEFAULT_REVERSE;
CRGB customColor1 = CRGB(100, 100, 100);
CRGB customColor2 = CRGB(0, 0, 100);
CRGB customColor3 = CRGB(0, 100, 100);

bool beat = false;
int bpm = DEFAULT_BPM;
unsigned long loopMillis = 0;
int beatLength(float multiplier = 1.0) {
  return 60000 / (bpm * multiplier); // eg. bpm 112 = 1 beat every 535.7 ms
}
int mapBeat(int start, int end, float multiplier = 1.0) {
  return map(loopMillis % beatLength(multiplier), 0, beatLength(multiplier),
             start, end);
}

unsigned long startTime = 0;
int nextAction = numActions; // actions dont fire until nextAction < numActions

// clang-format off
#include "twinkleUtils.h"
#include "ledDepth-ledAngle.h"
#include "overlay.h"

#include "Palette.h"
Palette palette;

#include "handleAction.h"

#if MODE == SINGLE_BOARD_MODE
#include "web_server.h"
#endif

#include "Spiral.h"
#include "SpiralSubPattern.h"
#include "Twinkle.h"
#include "TwinkleSubPattern.h"
#include "Line.h"
#include "LineSubPattern.h"
#include "LineBPM.h"
#include "LineSubPatternBPM.h"
#include "Flash.h"
#include "FlashSubPattern.h"
#include "SolidSubPattern.h"
// clang-format on

SolidSubPattern solid(SolidSubPattern::SOLID);
SolidSubPattern solidOverlay(SolidSubPattern::SOLID_OVERLAY);
SolidSubPattern solidOverlayRailgun(SolidSubPattern::SOLID_OVERLAY_RAILGUN);
TwinkleSubPattern twinkle(TwinkleSubPattern::TWINKLE);
TwinkleSubPattern
    randomFadingSegments(TwinkleSubPattern::RANDOM_FADING_SEGMENTS);
LineSubPattern randomFlashingSegments(LineSubPattern::RANDOM_FLASHING_SEGMENTS);
TwinkleSubPattern twinkleOverlay(TwinkleSubPattern::TWINKLE_OVERLAY);
SpiralSubPattern singleSpiral(SpiralSubPattern::SINGLE_SPIRAL);
SpiralSubPattern doubleSpiral(SpiralSubPattern::DOUBLE_SPIRAL);
LineSubPatternBPM rotatingPong(LineSubPatternBPM::ROTATING_PONG);
LineSubPatternBPM lasers(LineSubPatternBPM::LASERS);
LineSubPatternBPM lasersDoubles(LineSubPatternBPM::LASERS_DOUBLES);
LineSubPatternBPM lasersAllAtOnce(LineSubPatternBPM::LASERS_ALL_AT_ONCE);
LineSubPattern rainfall(LineSubPattern::RAINFALL);
LineSubPattern basketWeaving(LineSubPattern::BASKET_WEAVING);
LineSubPattern cometTrails(LineSubPattern::COMET_TRAILS);
LineSubPatternBPM rotatingHexagons(LineSubPatternBPM::ROTATING_HEXAGONS);
LineSubPatternBPM
    counterRotatingHexagons(LineSubPatternBPM::COUNTER_ROTATING_HEXAGONS);
LineSubPatternBPM
    variableSpeedRotation(LineSubPatternBPM::VARIABLE_SPEED_ROTATION);
LineSubPatternBPM
    variableSpeedRotationEnd(LineSubPatternBPM::VARIABLE_SPEED_ROTATION_END);
FlashSubPattern flashingHexagons(FlashSubPattern::FLASHING_HEXAGONS);
FlashSubPattern flashingHexagonsWarp(FlashSubPattern::FLASHING_HEXAGONS_WARP);

// clang-format off
SubPattern *activePatterns[] = {
  &solid,
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
  &lasersDoubles,
  &lasersAllAtOnce,
  &rainfall,
  &basketWeaving,
  &cometTrails,
  &rotatingHexagons,
  &counterRotatingHexagons,
  &variableSpeedRotation,
  &variableSpeedRotationEnd,
  &flashingHexagons,
  &flashingHexagonsWarp
};
// clang-format on

Timer actionTimer;
bool actionQueued = false;

void setup() {
  Serial.begin(9600);
  delay(500);
  randomSeed(analogRead(0));

#if MODE != SINGLE_BOARD_MODE
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
#endif

#if MODE == X8_RECEIVER_MODE

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

#else

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

#if MODE == SINGLE_BOARD_MODE
  setupWebServer();
#endif

  for (int i = 0; i < NUM_PATTERNS; i++) {
    activePatterns[i]->setup();
  }
}

void loop() {
  FastLED.clear();
  palette.cycle();

  // use loopMillis instead of millis so that everything calculated on one
  // iteration of a loop references the same point in time
  static unsigned long prevMillis = millis();
  loopMillis += millis() - prevMillis;
  prevMillis = millis();

  // set 'beat' to true for one loop iteration every time millis passes one
  // duration of bpm
  if (beat) {
    beat = false;
  }
  static int prevMod = 0;
  int mod = millis() % beatLength();
  if (mod < prevMod) {
    beat = true;
  }
  prevMod = mod;

#if MODE == SINGLE_BOARD_MODE
  EVERY_N_SECONDS(1) {
    // Serial.print("Local IP address: ");
    // Serial.println(WiFi.localIP());
  }

  unsigned long elapsedTime = millis() - startTime;
  if (elapsedTime >= actions[nextAction].timestamp && nextAction < numActions) {
    Serial.print("next action: ");
    Serial.println(nextAction);
    actions[nextAction].commitData();
    handleAction();
    nextAction++;
  }
#endif

  if (actionQueued && actionTimer.complete()) {
    handleAction();
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

  // static int prevSecond = -1;
  // static unsigned long prevTicks = ticks;
  // int currentSecond = millis() / 1000;
  // if (prevSecond != currentSecond) {
  //   Serial.print("Frames in last second: ");
  //   Serial.println(ticks - prevTicks);
  //   prevSecond = currentSecond;
  //   prevTicks = ticks;
  // }

  FastLED.setMaxRefreshRate(30);
  FastLED.setBrightness(brightness);
  FastLED.show();
  ticks++;
}

#if MODE != SINGLE_BOARD_MODE
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  actionQueued = true;
  actionTimer.totalCycleTime = data.delay;
  actionTimer.reset();
}
#endif
