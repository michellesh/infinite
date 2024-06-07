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
// clang-format on

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

#define MAX_VOLUME 21
#define MAX_FILES 100

#if MODE == X8_RECEIVER_MODE
#define NUM_RECEIVERS 8
#else
#define NUM_RECEIVERS 1
#endif

#define DELAY 50 // delay send between receivers

uint8_t receiverAddresses[NUM_RECEIVERS][6]; // 6 bytes in a mac address

esp_now_peer_info_t peerInfo;

Audio audio;

File rootDir;

char *fileNames[MAX_FILES];
int fileCount = 0;
int trackNumber = 0;
int receiverDelayAction = 0;
Timer trackDelayTimer;

void handleAction() {
  for (int i = 0; i < NUM_RECEIVERS; i++) {
#if MODE == X8_RECEIVER_MODE
    data.delay = (NUM_RECEIVERS - 1 - i) * DELAY + receiverDelayAction;
#elif MODE == X1_RECEIVER_MODE
    data.delay = DELAY * 7 +
                 receiverDelayAction; // Simulate the delay of all 8 receivers
#endif
    esp_err_t result =
        esp_now_send(receiverAddresses[i], (uint8_t *)&data, sizeof(msg));
    delay(DELAY);
  }
}

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
}

void loop() {
  audio.loop();

  // Execute next action
  static int nextAction = 0;
  static bool trackQueued = false;
  if (millis() >= actions[nextAction].timestamp && nextAction < numActions) {
    actions[nextAction].commitData();
    handleAction();
    if (actions[nextAction].setTrack) {
      if (actions[nextAction].trackDelay > 0) {
        // delay playing the track
        trackQueued = true;
        trackDelayTimer.totalCycleTime = actions[nextAction].trackDelay;
        trackDelayTimer.reset();
        receiverDelayAction = 0;
      } else if (actions[nextAction].trackDelay < 0) {
        // delay sending the actions
        playTrack(actions[nextAction].trackNumber);
        receiverDelayAction = abs(actions[nextAction].trackDelay);
      } else {
        // neither track nor action delay
        playTrack(actions[nextAction].trackNumber);
        receiverDelayAction = 0;
      }
    }
    nextAction++;
  }

  if (trackQueued && trackDelayTimer.complete()) {
    playTrack(actions[nextAction].trackNumber);
    trackQueued = false;
  }

  // Execute next volume fade
  /*
  static int volume = MAX_VOLUME;
  static int prevVolume = MAX_VOLUME;
  static int nextFade = 0;
  if (nextFade < numFades) {
    if (millis() >= fades[nextFade].millisStart &&
        millis() <= fades[nextFade].millisEnd) {
      volume = fades[nextFade].getVolume();
    } else if (millis() > fades[nextFade].millisEnd) {
      nextFade++;
    }
  }
  if (volume != prevVolume) {
    Serial.print("volume: ");
    Serial.println(volume);
    audio.setVolume(volume);
    prevVolume = volume;
  }
  */
}

void playTrack(int index) {
  Serial.print("playing ");
  Serial.println(fileNames[index]);
  audio.connecttoFS(SD, fileNames[index]);
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
  audio.setVolume(MAX_VOLUME);

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

/*
void readSerialMonitorInput() {
  if (Serial.available() > 0) {
    // Read incoming string until '\n' (newline) character is received
    String receivedString = Serial.readStringUntil('\n');
    // Print received string
    Serial.print("Received: ");
    Serial.println(receivedString);

    // Find the position of the ':' character
    int colonIndex = receivedString.indexOf(':');

    // If ':' character is found
    if (colonIndex != -1) {
      // Extract the substrings before and after the ':'
      String firstPart = receivedString.substring(0, colonIndex);
      String secondPart = receivedString.substring(colonIndex + 1);

      // Parse the substrings into integers
      int action = firstPart.toInt();
      int value = secondPart.toInt();

      handleAction(action, value);

    } else {
      // If ':' character is not found, set both integers to 0
      Serial.println("No ':' character found");
    }
  }
}
*/
