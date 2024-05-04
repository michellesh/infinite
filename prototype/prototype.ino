// clang-format off
#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>
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

#define STRIP_LENGTH 144
#define NUM_STRIPS 16
#define NUM_LEDS STRIP_LENGTH * NUM_STRIPS

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  delay(500);

  FastLED.addLeds<WS2813, PIN_1, GRB>(leds, 0, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_2, GRB>(leds, STRIP_LENGTH, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_3, GRB>(leds, STRIP_LENGTH * 2, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_4, GRB>(leds, STRIP_LENGTH * 3, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_5, GRB>(leds, STRIP_LENGTH * 4, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_6, GRB>(leds, STRIP_LENGTH * 5, STRIP_LENGTH)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_7, GRB>(leds, STRIP_LENGTH * 6, STRIP_LENGTH * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_8, GRB>(leds, STRIP_LENGTH * 8, STRIP_LENGTH * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_9, GRB>(leds, STRIP_LENGTH * 10, STRIP_LENGTH * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_10, GRB>(leds, STRIP_LENGTH * 12, STRIP_LENGTH * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
  FastLED.addLeds<WS2813, PIN_11, GRB>(leds, STRIP_LENGTH * 14, STRIP_LENGTH * 2)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);
}

void loop() {
  FastLED.clear();

  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < STRIP_LENGTH; j++) {
      int hue = map(i, 0, NUM_STRIPS, 100, 180);
      leds[i * STRIP_LENGTH + j] = CHSV(hue, 170, 255);
    }
  }

  FastLED.show();
}
