// clang-format off
#include <WiFi.h>
#include <esp_now.h>

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

#if MODE == X8_RECEIVER_MODE
#define NUM_RECEIVERS 8
#else
#define NUM_RECEIVERS 1
#endif

#define DELAY 50 // delay send between receivers

#define LED_PIN 18

uint8_t receiverAddresses[NUM_RECEIVERS][6]; // 6 bytes in a mac address

bool autoCyclePalettes = DEFAULT_AUTOCYCLEPALETTES;

msg data;

#include "Action.h"
#include "Fade.h"

Action time(unsigned long timestamp) {
  Action a = {timestamp};
  return a;
}

Fade fadeIn(unsigned long millisStart, unsigned long millisEnd) {
  Fade f = {millisStart, millisEnd, 0, MAX_VOLUME};
  return f;
}

Fade fadeOut(unsigned long millisStart, unsigned long millisEnd) {
  Fade f = {millisStart, millisEnd, MAX_VOLUME, 0};
  return f;
}

String getTrackName(int trackNumber) {
  switch (trackNumber) {
  case 1:
    return "KILLER QUEEN";
  case 2:
    return "BOHEMIAN RHAPSODY";
  case 3:
    return "FLASH";
  case 4:
    return "FAT BOTTOMED GIRLS";
  case 5:
    return "ANOTHER ONE BITES THE DUST";
  case 6:
    return "BICYCLE RACE";
  case 7:
    return "YOU'RE MY BEST FRIEND";
  case 8:
    return "DONT STOP ME NOW";
  case 9:
    return "SAVE ME";
  case 10:
    return "CRAZY LITTLE THING CALLED LOVE";
  default:
    return "";
  }
}

int getBPM(int trackNumber) {
  switch (trackNumber) {
  case 1:
    return 118; // -> 3 killer queen
  case 2:
    return 141; // -> 1 bohemian rhapsody
  case 3:
    return 108; // -> 14 flash
  case 4:
    return 88; // -> 4 fat bottomed girls
  case 5:
    return 112; // -> 2 another one bites the dust
  case 6:
    return 168; // -> 5 bicycle race
  case 7:
    return 119; // -> 6 you're my best friend
  case 8:
    return 156; // -> 7 dont stop me now
  case 9:
    return 82; // -> 8 save me
  case 10:
    return 77; // -> 9 crazy little thing called love
  default:
    return 0;
  }
}

#include "actions.h"

int numActions = sizeof(actions) / sizeof(actions[0]);
int numFades = sizeof(fades) / sizeof(fades[0]);

esp_now_peer_info_t peerInfo;

Timer paletteCycleTimer = {DEFAULT_SECONDSPERPALETTE * 1000};
Timer oneSecondTimer = {1000};
Timer ledTimer;

void handleAction() {
  for (int i = 0; i < NUM_RECEIVERS; i++) {
#if MODE == X8_RECEIVER_MODE
    data.delay = (NUM_RECEIVERS - 1 - i) * DELAY;
#elif MODE == X1_RECEIVER_MODE
    data.delay = DELAY * 7; // Simulate the delay of all 8 receivers
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
  Serial2.begin(9600);
  delay(200);

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

  mp3_command(CMD_SEL_DEV, DEV_TF); // select the TF card
  delay(200);                       // wait for 200ms

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // static int palette = 0;
  // if (autoCyclePalettes && paletteCycleTimer.complete()) {
  //   palette = (palette + 1) % NUM_PALETTES;
  //   handleAction(ACTION_SET_PALETTE, palette);
  //   paletteCycleTimer.reset();
  // }

  if (oneSecondTimer.complete()) {
    Serial.print("s: ");
    Serial.println(millis() / 1000);
    oneSecondTimer.reset();
  }

  static int currentBPM = 0;

  // Execute next action
  static int nextAction = 0;
  if (millis() >= actions[nextAction].timestamp && nextAction < numActions) {
    Serial.print("next action: ");
    Serial.println(nextAction);
    actions[nextAction].commitData();
    handleAction();
    if (actions[nextAction].setTrack) {
      mp3_command(CMD_PLAY_W_INDEX, actions[nextAction].trackNumber);
      currentBPM = getBPM(actions[nextAction].trackNumber);
      Serial.println(getTrackName(actions[nextAction].trackNumber));
    }
    nextAction++;
  }

  // Execute next volume fade
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
    mp3_command(CMD_SET_VOLUME, volume);
    prevVolume = volume;
  }

  // Blink an LED to the beat
  if (currentBPM != 0) {
    unsigned long msPerBeat = 60000 / currentBPM; // one minute is 60000 ms
    unsigned long flashDuration = 100;
    static bool ledOn = false;
    if (ledTimer.complete()) {
      ledOn = !ledOn;
      if (ledOn) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
      ledTimer.totalCycleTime =
          ledOn ? flashDuration : msPerBeat - flashDuration;
      ledTimer.reset();
    }
  }

  // readSerialMonitorInput();
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