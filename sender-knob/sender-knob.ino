// clang-format off
#include "Arduino.h"
#include "Audio.h" // https://github.com/schreibfaul1/ESP32-audioI2S
#include "FS.h"
#include "SD.h"

#include <FastLED.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Timer.h"

#include "InfiniteShared.h"
#include "sequences.h"
// clang-format on

// Knob LEDs
#define LED_PIN 13
#define NUM_LEDS 24

CRGB leds[NUM_LEDS];

// Rotary encoder
#define ENCODER_CLK 33
#define ENCODER_DT 32
#define ENCODER_SW 34
#define MAX_CLICKS 40
#define NUM_KNOB_POSITIONS 8

int clicks = 0;
int currentCLK;
int prevCLK;
int currentSW = 0;
int prevSW = 0;
int knobPosition = 0;
int numSpins = 3;
float spinPosition = NUM_LEDS * numSpins;
int volume = MAX_VOLUME;
bool buttonHeld = false;

// SD Card
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

// MAX98357 audio amplifier
// GAIN pin tied to ground
// SD pin floating
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

#define MAX_FILES 100

#if MODE == X8_RECEIVER_MODE
#define NUM_RECEIVERS 8
#else
#define NUM_RECEIVERS 1
#endif

#define DELAY 50                    // delay send between receivers
#define LONG_PRESS_BUTTON_TIMER 300 // duration of long-press button

uint8_t receiverAddresses[NUM_RECEIVERS][6]; // 6 bytes in a mac address

esp_now_peer_info_t peerInfo;

Audio audio;

File rootDir;

char *fileNames[MAX_FILES];
int fileCount = 0;
int trackNumber = 0;
int receiverDelayAction = 0;
Timer trackDelayTimer;
Timer delaySendTimer = {DELAY};
Timer longPressButtonTimer = {LONG_PRESS_BUTTON_TIMER};

int standbySequenceIndex = numSequences - 1;
int sequenceIndex = standbySequenceIndex;
Sequence *sequence = sequences[sequenceIndex];

// callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
           mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

void macAddressStrToBytes(const char *macStr, uint8_t *macBytes) {
  for (int i = 0; i < 6; ++i) {
    macBytes[i] = strtoul(macStr + 3 * i, nullptr, 16);
  }
}

void registerPeer(uint8_t *receiverAddress) {
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.print("Failed to add peer");
    return;
  }
}

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  // Setup SD card and audio library
  setupAudioSD();

  // Print out list of files on SD card
  populateFileNames(rootDir);

  // Setup receivers
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  // register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  for (int i = 0; i < NUM_RECEIVERS; i++) {
    macAddressStrToBytes(receivers[i], receiverAddresses[i]);
    registerPeer(receiverAddresses[i]);
  }

  setupRotaryEncoder();
}

void loop() {
  audio.loop();
  audio.setVolume(volume);

  readRotaryEncoder();

  static unsigned long startTime = millis();
  static int prevSequenceIndex = sequenceIndex;
  static int nextAction = 0;
  if (prevSequenceIndex != sequenceIndex) {
    startTime = millis();
    nextAction = 0;
    prevSequenceIndex = sequenceIndex;
  }

  // Execute next action
  static bool trackQueued = false;
  static char *queuedFilename;
  static int nextReceiver = NUM_RECEIVERS;
  unsigned long elapsedTime = millis() - startTime;
  bool isValidSequence = sequenceIndex >= 0 && sequenceIndex < numSequences;
  if (isValidSequence &&
      elapsedTime >= sequence->actions[nextAction].timestamp &&
      nextAction < sequence->numActions) {
    sequence->actions[nextAction].commitData();
    nextReceiver = 0;
    if (sequence->actions[nextAction].setTrack) {
      if (sequence->actions[nextAction].trackDelay > 0) {
        // delay playing the track
        trackQueued = true;
        queuedFilename = sequence->actions[nextAction].filename;
        trackDelayTimer.totalCycleTime =
            sequence->actions[nextAction].trackDelay;
        trackDelayTimer.reset();
        receiverDelayAction = 0;
      } else if (sequence->actions[nextAction].trackDelay < 0) {
        // delay sending the actions
        playTrack(sequence->actions[nextAction].filename);
        receiverDelayAction = abs(sequence->actions[nextAction].trackDelay);
      } else {
        // neither track nor action delay
        playTrack(sequence->actions[nextAction].filename);
        receiverDelayAction = 0;
      }
    }
    nextAction++;
  }

  if (nextReceiver < NUM_RECEIVERS) {
    if (delaySendTimer.complete()) {
#if MODE == X8_RECEIVER_MODE
      data.delay =
          (NUM_RECEIVERS - 1 - nextReceiver) * DELAY + receiverDelayAction;
#elif MODE == X1_RECEIVER_MODE
      data.delay = DELAY * 7 +
                   receiverDelayAction; // Simulate the delay of all 8 receivers
#endif
      esp_err_t result = esp_now_send(receiverAddresses[nextReceiver],
                                      (uint8_t *)&data, sizeof(msg));

      nextReceiver++;
      delaySendTimer.reset();
    }
  }

  if (trackQueued && trackDelayTimer.complete()) {
    playTrack(queuedFilename);
    trackQueued = false;
  }

  showKnobLEDs();
}

void audio_eof_mp3(const char *info) { // end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
  setSequence(standbySequenceIndex);
}

void resetDefaultData() {
  data.delay = 0;
  data.pattern = DEFAULT_PATTERN;
  data.palette = DEFAULT_PALETTE;
  data.colorMode = DEFAULT_COLORMODE;
  data.speed = DEFAULT_SPEED;
  data.density = DEFAULT_DENSITY;
  data.width = DEFAULT_WIDTH;
  data.overlaySpeed = DEFAULT_OVERLAYSPEED;
  data.overlayDensity = DEFAULT_OVERLAYDENSITY;
  data.overlayWidth = DEFAULT_OVERLAYWIDTH;
  data.reverse = DEFAULT_REVERSE;
  data.bpm = DEFAULT_BPM;
  data.overlay = 0;
  data.fadeMillis = DEFAULT_FADE_MILLIS;
}

void playTrack(char *filename) {
  Serial.print("playing ");
  Serial.println(filename);
  audio.connecttoFS(SD, filename);
}

void populateFileNames(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // No more files
      break;
    }

    if (!entry.isDirectory()) {
      // Check if the file has a .mp3 extension
      const char *fileName = entry.name();
      if (fileName[0] == '.') {
        // Serial.print("Ignoring file: ");
        // Serial.println(fileName);
        continue;
      }

      const char *extension =
          strrchr(fileName, '.'); // Find the last occurrence of '.'
      if (extension != nullptr &&
          (strcmp(extension, ".mp3") == 0 || strcmp(extension, ".wav") == 0)) {
        // Store the file name
        if (fileCount < MAX_FILES) {
          fileNames[fileCount] =
              strdup(fileName); // Allocate memory and copy the name
          fileCount++;
        } else {
          // Handle case where there are more files than the array can hold
          Serial.println("Too many files to store in array");
          break;
        }
      }
    }
    entry.close();
  }

  // Print the file names
  Serial.println("MP3 files found on SD card:");
  for (int i = 0; i < fileCount; i++) {
    Serial.print("Track ");
    Serial.print(i);
    Serial.print(": \"");
    Serial.print(fileNames[i]);
    Serial.println("\"");
  }
}

void setupAudioSD() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed!");
    return;
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(volume);

  rootDir = SD.open("/");
  if (!rootDir) {
    Serial.println("Failed to open root directory");
    return;
  }
  // Ensure the directory is a valid directory
  if (!rootDir.isDirectory()) {
    Serial.println("Root is not a directory");
    return;
  }
}

void setupRotaryEncoder() {
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT);

  // Read the initial state
  prevCLK = digitalRead(ENCODER_CLK);
  prevSW = digitalRead(ENCODER_SW);
}

void setSequence(int index) {
  sequenceIndex = index;
  sequence = sequences[sequenceIndex];
  resetDefaultData();
  if (index == standbySequenceIndex) {
    audio.stopSong();
  }
}

void readRotaryEncoder() {
  // Read the current state
  currentCLK = digitalRead(ENCODER_CLK);
  currentSW = digitalRead(ENCODER_SW);

  if (currentSW == HIGH && prevSW == LOW) {
    Serial.println("Button up!");
    buttonHeld = false;
    if (!longPressButtonTimer.complete()) {
      // Set sequence according to knob position
      if (knobPosition < numSequences) {
        setSequence(knobPosition);
        if (knobPosition < numSequences - 1) {
          // Play a spinning animation if non-standby sequence selected
          spinPosition = 0;
        }
      }
    }
  }

  if (currentSW == LOW && prevSW == HIGH) {
    Serial.println("Button down!");
    longPressButtonTimer.reset();
    buttonHeld = true;
  }

  // A pulse occurs if the previous and the current state differ
  int delta = 0;
  if (currentCLK != prevCLK) {
    if (digitalRead(ENCODER_DT) != currentCLK) {
      clicks++;
      delta = 1;
      if (clicks >= MAX_CLICKS) {
        clicks = 0;
      }
    } else {
      clicks--;
      delta = -1;
      if (clicks < 0) {
        clicks = MAX_CLICKS;
      }
    }

    if (longPressButtonTimer.complete() && buttonHeld) {
      // change volume
      volume = constrain(volume + delta, 0, MAX_VOLUME);
      Serial.print("Volume: ");
      Serial.println(volume);
    } else {
      // change knob position
      knobPosition = map(clicks, 0, MAX_CLICKS, 0, NUM_KNOB_POSITIONS);
      Serial.print("Knob position: ");
      Serial.println(map(clicks, 0, MAX_CLICKS, 0, NUM_KNOB_POSITIONS));
    }
  }

  prevCLK = currentCLK;
  prevSW = currentSW;
}

void showKnobLEDs() {
  if (spinPosition < NUM_LEDS * numSpins) {
    fadeToBlackBy(leds, NUM_LEDS, 10);
  } else {
    FastLED.clear();
  }

  if (spinPosition < NUM_LEDS * numSpins) {
    // Show a spinning animation when sequence is selected
    int index = int(spinPosition) % NUM_LEDS;
    leds[NUM_LEDS - index - 1] = CRGB::White;
    spinPosition += 0.5;
  } else if (longPressButtonTimer.complete() && buttonHeld) {
    // Show volume meter
    int maxIndex = map(volume, 0, MAX_VOLUME, 0, NUM_LEDS);
    for (int i = 0; i < maxIndex; i++) {
      leds[NUM_LEDS - i - 1] = CRGB::White;
    }
  } else if (knobPosition < numSequences - 1) {
    // Light segment of LEDs aligned with current knob position
    int numLEDsPerKnobPosition = NUM_LEDS / NUM_KNOB_POSITIONS;
    int startIndex = knobPosition * numLEDsPerKnobPosition;
    int hue = map(knobPosition, 0, NUM_KNOB_POSITIONS, 0, 255);
    for (int i = startIndex; i < startIndex + numLEDsPerKnobPosition; i++) {
      leds[NUM_LEDS - i - 1] = CHSV(hue, 255, 255);
    }
  }

  FastLED.show();
}