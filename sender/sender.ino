// Source https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
// clang-format off
#include <WiFi.h>
#include <esp_now.h>

#include "InfiniteShared.h"
// clang-format on

int speed = DEFAULT_SPEED;
int density = DEFAULT_DENSITY;
int width = DEFAULT_WIDTH;
int overlaySpeed = DEFAULT_OVERLAYSPEED;
int overlayWidth = DEFAULT_OVERLAYWIDTH;
int overlayDensity = DEFAULT_OVERLAYDENSITY;
bool reverse = DEFAULT_REVERSE;
bool autoCyclePalettes = DEFAULT_AUTOCYCLEPALETTES;
int secondsPerPalette = DEFAULT_SECONDSPERPALETTE;

msg data;

void handleAction(uint8_t action, int value = 0) {
  Serial.print("action: ");
  Serial.println(action);
  Serial.print("value: ");
  Serial.println(value);

  data.action = action;
  data.value = value;

  esp_err_t result = send(data);

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
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
  // register first receiver
  memcpy(peerInfo.peer_addr, receiverAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // register second receiver
  memcpy(peerInfo.peer_addr, receiverAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
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
