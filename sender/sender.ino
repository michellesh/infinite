// Source https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
// clang-format off
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Time.h>

#include "Timer.h"

#include "InfiniteShared.h"
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

#define NUM_RECEIVERS 8
#define DELAY 100 // delay send between receivers

uint8_t receiverAddresses[NUM_RECEIVERS][6]; // 6 bytes in a mac address

int speed = DEFAULT_SPEED;
int density = DEFAULT_DENSITY;
int width = DEFAULT_WIDTH;
int overlaySpeed = DEFAULT_OVERLAYSPEED;
int overlayWidth = DEFAULT_OVERLAYWIDTH;
int overlayDensity = DEFAULT_OVERLAYDENSITY;
bool reverse = DEFAULT_REVERSE;
bool autoCyclePalettes = DEFAULT_AUTOCYCLEPALETTES;

msg data;

esp_now_peer_info_t peerInfo;

Timer paletteCycleTimer = {DEFAULT_SECONDSPERPALETTE * 1000};
Timer oneSecondTimer = {1000};

ESP32Time rtc;

void handleAction(uint8_t action, int value = 0) {
  Serial.print("action: ");
  Serial.println(action);
  Serial.print("value: ");
  Serial.println(value);

  if (action == ACTION_SET_AUTO_CYCLE_PALETTES) {
    autoCyclePalettes = value;
    return;
  } else if (action == ACTION_SET_SECONDS_PER_PALETTE) {
    paletteCycleTimer.totalCycleTime = value * 1000;
    autoCyclePalettes = false;
    return;
  }

  data.action = action;
  data.value = value;

  for (int i = 0; i < NUM_RECEIVERS; i++) {
    data.delay = (NUM_RECEIVERS - 1 - i) * DELAY;
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
  Serial2.begin(9600);
  delay(200);

  rtc.setTime(30, 24, 15, 17, 1, 2024); // 17th Jan 2024 15:24:30

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  // register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  macAddressStrToBytes(receiverM1, receiverAddresses[0]);
  macAddressStrToBytes(receiverM2, receiverAddresses[1]);
  macAddressStrToBytes(receiverM3, receiverAddresses[2]);
  macAddressStrToBytes(receiverM4, receiverAddresses[3]);
  macAddressStrToBytes(receiverM5, receiverAddresses[4]);
  macAddressStrToBytes(receiverM6, receiverAddresses[5]);
  macAddressStrToBytes(receiverM7, receiverAddresses[6]);
  macAddressStrToBytes(receiverM8, receiverAddresses[7]);

  registerPeer(receiverAddresses[0]);
  registerPeer(receiverAddresses[1]);
  registerPeer(receiverAddresses[2]);
  registerPeer(receiverAddresses[3]);
  registerPeer(receiverAddresses[4]);
  registerPeer(receiverAddresses[5]);
  registerPeer(receiverAddresses[6]);
  registerPeer(receiverAddresses[7]);

  mp3_command(CMD_SEL_DEV, DEV_TF); // select the TF card
  delay(2000);                       // wait for 200ms
}

void loop() {
  static int palette = 0;
  if (autoCyclePalettes && paletteCycleTimer.complete()) {
    palette = (palette + 1) % NUM_PALETTES;
    handleAction(ACTION_SET_PALETTE, palette);
    paletteCycleTimer.reset();
  }

  // Update milliseconds
  static unsigned long ms = 0;
  static unsigned long prevMs = 0;
  unsigned long currentMs = rtc.getMillis();
  if (currentMs > prevMs) {
    ms += (currentMs - prevMs);
  } else if (prevMs > currentMs) {
    ms += currentMs + (1000 - prevMs);
  }
  prevMs = currentMs;

  if (oneSecondTimer.complete()) {
    Serial.print("s: ");
    Serial.println(ms / 1000);
    oneSecondTimer.reset();
  }

  // trigger track at given timestamps
  static int nextTrack = 0;
  int trackPlayTimes[] = {0, 5000, 10000, 15000};
  int patternPlayTimes[] = {0, PATTERN_SOLID_OVERLAY, PATTERN_ROTATING_HEXAGONS, PATTERN_TWINKLE};
  int numTracks = sizeof(trackPlayTimes) / sizeof(trackPlayTimes[0]);
  if (ms >= trackPlayTimes[nextTrack] && nextTrack < numTracks) {
    Serial.print("play index: ");
    Serial.println(nextTrack);
    mp3_command(CMD_PLAY_NEXT, 0x0000); // Play next mp3
    handleAction(ACTION_SET_PATTERN, patternPlayTimes[nextTrack]); // Play pattern
    nextTrack++;
  }

  //readSerialMonitorInput();
}

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
