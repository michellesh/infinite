/*
 * Source:
 * https://esp32io.com/tutorials/esp32-mp3-player
 *
 * Using micro SDHC card 16GB
 * mp3 files stored in root directory
 *
 *  WIRING:
 *
 *  ESP32         | MP3 player module
 * ---------------+-------------------
 *  Vin           | VCC
 *  GND           | GND
 *  RX2 (GPIO 16) | TX
 *  TX2 (GPIO 17) | RX
 */
// clang-format off
#include <FastLED_NeoMatrix.h>
#include <arduinoFFT.h>
#include <ESP32Time.h>
// clang-format on

#define CMD_PLAY_NEXT 0x01
#define CMD_PLAY_PREV 0x02
#define CMD_PLAY_W_INDEX 0x03
#define CMD_SET_VOLUME 0x06
#define CMD_SEL_DEV 0x09
#define CMD_PLAY_W_VOL 0x22
#define CMD_PLAY 0x0D
#define CMD_PAUSE 0x0E
#define CMD_SINGLE_CYCLE 0x19

#define DEV_TF 0x02
#define SINGLE_CYCLE_ON 0x00
#define SINGLE_CYCLE_OFF 0x01

#define MAX_VOLUME 30

#define AUDIO_IN_PIN 35
#define SAMPLES 1024
#define SAMPLING_FREQ 40000

#define SAMPLES         1024          // Must be a power of 2
#define SAMPLING_FREQ   10240         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE       1000          // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define AUDIO_IN_PIN    35            // Signal in on this pin
#define NUM_BANDS       16            // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE           500           // Used as a crude noise filter, values below this are ignored
#define TOP            16                // Don't allow the bars to go offscreen

#define MAX_MILLIAMPS   2000          // Careful with the amount of power here if running off USB port
const int BRIGHTNESS_SETTINGS[3] = {5, 70, 200};  // 3 Integer array for 3 brightness settings (based on pressing+holding BTN_PIN)
#define LED_VOLTS       5             // Usually 5 or 12
#define CHIPSET         WS2812B       // LED strip type
#define COLOR_ORDER     GRB           // If colours look wrong, play with this
#define LED_PIN         13             // LED strip data
const uint8_t kMatrixWidth = 16;                          // Matrix width
const uint8_t kMatrixHeight = 16;                         // Matrix height
#define NUM_LEDS       (kMatrixWidth * kMatrixHeight)     // Total number of LEDs
#define BAR_WIDTH      (kMatrixWidth  / (NUM_BANDS - 1))  // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc

// Sampling and FFT stuff
unsigned int sampling_period_us;
byte peak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};              // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int bandValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
//ArduinoFFT FFT = ArduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

ESP32Time rtc;

CRGB leds[NUM_LEDS];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, kMatrixWidth, kMatrixHeight,
  NEO_MATRIX_TOP        + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS       + NEO_MATRIX_ZIGZAG +
  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(500); // wait chip initialization is complete

  rtc.setTime(30, 24, 15, 17, 1, 2024); // 17th Jan 2024 15:24:30

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS_SETTINGS[1]);
  FastLED.clear();

  mp3_command(CMD_SEL_DEV, DEV_TF); // select the TF card
  delay(200);                       // wait for 200ms

  // mp3_command(CMD_PLAY, 0x0000); // Play first track

  // mp3_command(CMD_SINGLE_CYCLE, SINGLE_CYCLE_ON); // Plays the song on repeat
  // mp3_command(CMD_PLAY_W_INDEX, 3);               // Play track number 3
  // mp3_command(CMD_PAUSE, 0x0000);      // Pause mp3
  // mp3_command(CMD_PLAY_NEXT, 0x0000);  // Play next mp3
  // mp3_command(CMD_PLAY_PREV, 0x0000);  // Play previous mp3
  // mp3_command(CMD_SET_VOLUME, 30);     // Change volume to 30

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

void loop() {

  FastLED.clear();

  // Reset bandValues[]
  for (int i = 0; i<NUM_BANDS; i++){
    bandValues[i] = 0;
  }

  // Sample the audio pin
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us) { /* chill */ }
  }

  // Compute FFT
  FFT.dcRemoval();
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES/2); i++){       // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE) {                    // Add a crude noise filter

    /*8 bands, 12kHz top band
      if (i<=3 )           bandValues[0]  += (int)vReal[i];
      if (i>3   && i<=6  ) bandValues[1]  += (int)vReal[i];
      if (i>6   && i<=13 ) bandValues[2]  += (int)vReal[i];
      if (i>13  && i<=27 ) bandValues[3]  += (int)vReal[i];
      if (i>27  && i<=55 ) bandValues[4]  += (int)vReal[i];
      if (i>55  && i<=112) bandValues[5]  += (int)vReal[i];
      if (i>112 && i<=229) bandValues[6]  += (int)vReal[i];
      if (i>229          ) bandValues[7]  += (int)vReal[i];*/

    //16 bands, 12kHz top band
      if (i<=2 )           bandValues[0]  += (int)vReal[i];
      if (i>2   && i<=3  ) bandValues[1]  += (int)vReal[i];
      if (i>3   && i<=5  ) bandValues[2]  += (int)vReal[i];
      if (i>5   && i<=7  ) bandValues[3]  += (int)vReal[i];
      if (i>7   && i<=9  ) bandValues[4]  += (int)vReal[i];
      if (i>9   && i<=13 ) bandValues[5]  += (int)vReal[i];
      if (i>13  && i<=18 ) bandValues[6]  += (int)vReal[i];
      if (i>18  && i<=25 ) bandValues[7]  += (int)vReal[i];
      if (i>25  && i<=36 ) bandValues[8]  += (int)vReal[i];
      if (i>36  && i<=50 ) bandValues[9]  += (int)vReal[i];
      if (i>50  && i<=69 ) bandValues[10] += (int)vReal[i];
      if (i>69  && i<=97 ) bandValues[11] += (int)vReal[i];
      if (i>97  && i<=135) bandValues[12] += (int)vReal[i];
      if (i>135 && i<=189) bandValues[13] += (int)vReal[i];
      if (i>189 && i<=264) bandValues[14] += (int)vReal[i];
      if (i>264          ) bandValues[15] += (int)vReal[i];
    }
  }

  int sum = 0;
  // Process the FFT data into bar heights
  for (byte band = 0; band < NUM_BANDS; band++) {

    // Scale the bars for the display
    int barHeight = bandValues[band] / AMPLITUDE;
    if (barHeight > TOP) barHeight = TOP;

    // Small amount of averaging between frames
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

    // Move peak up
    if (barHeight > peak[band]) {
      peak[band] = min(TOP, barHeight);
    }

    sum += barHeight;
    // Draw bars
    rainbowBars(band, barHeight);

    // Draw peaks

    // Save oldBarHeights for averaging later
    oldBarHeights[band] = barHeight;
  }

  // Update milliseconds
  static unsigned long ms = 0;
  static unsigned long prevMs = 0;
  unsigned long currentMs = rtc.getMillis();
  if (currentMs > prevMs) {
    ms += (currentMs - prevMs);
  } else {
    ms += currentMs + (1000 - prevMs);
  }
  prevMs = currentMs;

  EVERY_N_SECONDS(1) {
    Serial.print("s: ");
    Serial.println(ms / 1000);
  }

  // trigger track at given timestamps
  static int nextTrack = 0;
  int trackPlayTimes[] = {0, 1000, 12000, 15000};
  if (ms >= trackPlayTimes[nextTrack]) {
    Serial.print("play next: ");
    Serial.println(nextTrack);
    mp3_command(CMD_PLAY_NEXT, 0x0000);  // Play next mp3
    nextTrack++;
  }

  //16 stop, 33 start, 50 stop, 68 start, 86 stop
  //48s,     99s,      150s,    204s,     258s
  //every 50s

  static int volume = MAX_VOLUME;
  static int prevVolume = MAX_VOLUME;

  static int nextFade = 0;
  int fade[][4] = {{5000, 10000, MAX_VOLUME, 0}, {11000, 16000, 0, MAX_VOLUME}};
  if (ms >= fade[nextFade][0] && ms <= fade[nextFade][1]) {
    volume = map(ms, fade[nextFade][0], fade[nextFade][1], fade[nextFade][2], fade[nextFade][3]);
  } else if (ms > fade[nextFade][1]) {
    nextFade++;
  }
  // fade out between 5 and 10 second timestamps
  //if (ms >= 5000 && ms <= 10000) {
  //  volume = map(ms, 5000, 10000, MAX_VOLUME, 0);
  //}

  // fade in between 15 and 20 second timestamps
  //if (ms >= 13000 && ms <= 18000) {
  //  volume = map(ms, 13000, 18000, 0, MAX_VOLUME);
  //}

  //static int nextCount = 0;
  //EVERY_N_SECONDS(5) {
  //  Serial.println("CMD_PLAY_NEXT");
  //  mp3_command(CMD_PLAY_NEXT, 0x0000);  // Play next mp3
  //  nextCount++;
  //  Serial.print("nextCount: ");
  //  Serial.println(nextCount);
  //}

  if (volume != prevVolume) {
    Serial.print("volume: ");
    Serial.println(volume);
    mp3_command(CMD_SET_VOLUME, volume);
    prevVolume = volume;
  }

  FastLED.show();
}

void rainbowBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, CHSV((x / BAR_WIDTH) * (255 / NUM_BANDS), 255, 255));
    }
  }
}

void mp3_command(int8_t command, int16_t dat) {
  int8_t frame[8] = {0};
  frame[0] = 0x7e; // starting byte
  frame[1] = 0xff; // version
  frame[2] = 0x06; // the number of bytes of the command without starting byte
                   // and ending byte
  frame[3] = command;            //
  frame[4] = 0x00;               // 0x00 = no feedback, 0x01 = feedback
  frame[5] = (int8_t)(dat >> 8); // data high byte
  frame[6] = (int8_t)(dat);      // data low byte
  frame[7] = 0xef;               // ending byte
  for (uint8_t i = 0; i < 8; i++) {
    Serial2.write(frame[i]);
  }
}
