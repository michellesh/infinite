// Source https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
// clang-format off
#include <WiFi.h>
#include <esp_now.h>

#include "Timer.h"

#include "InfiniteShared.h"
// clang-format on

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

Timer paletteCycleTimer = {DEFAULT_SECONDSPERPALETTE * 1000};

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

esp_now_peer_info_t peerInfo;

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
}

void loop() {
  static int palette = 0;
  if (autoCyclePalettes && paletteCycleTimer.complete()) {
    palette = (palette + 1) % NUM_PALETTES;
    handleAction(ACTION_SET_PALETTE, palette);
    paletteCycleTimer.reset();
  }

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
