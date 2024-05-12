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

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(500); // wait chip initialization is complete

  mp3_command(CMD_SEL_DEV, DEV_TF); // select the TF card
  delay(200);                       // wait for 200ms

  mp3_command(CMD_SINGLE_CYCLE, SINGLE_CYCLE_ON); // Plays the song on repeat
  mp3_command(CMD_PLAY_W_INDEX, 3);               // Play track number 3

  // mp3_command(CMD_PAUSE, 0x0000);      // Pause mp3
  // mp3_command(CMD_PLAY_NEXT, 0x0000);  // Play next mp3
  // mp3_command(CMD_PLAY_PREV, 0x0000);  // Play previous mp3
  // mp3_command(CMD_SET_VOLUME, 30);     // Change volume to 30
}

void loop() {}

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
