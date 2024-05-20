#define ACTION_SET_PATTERN 0
#define ACTION_SET_PALETTE 1
#define ACTION_SET_AUTO_CYCLE_PALETTES 2
#define ACTION_SET_SECONDS_PER_PALETTE 3
#define ACTION_SET_COLOR_MODE 4
#define ACTION_SET_SPEED 5
#define ACTION_SET_DENSITY 6
#define ACTION_SET_WIDTH 7
#define ACTION_SET_OVERLAY_SPEED 8
#define ACTION_SET_OVERLAY_WIDTH 9
#define ACTION_SET_OVERLAY_DENSITY 10
#define ACTION_TOGGLE_REVERSE 11

#define PATTERN_SOLID_OVERLAY 0
#define PATTERN_SOLID_OVERLAY_RAILGUN 1
#define PATTERN_TWINKLE 2
#define PATTERN_RANDOM_FADING_SEGMENTS 3
#define PATTERN_RANDOM_FLASHING_SEGMENTS 4
#define PATTERN_TWINKLE_OVERLAY 5
#define PATTERN_SINGLE_SPIRAL 6
#define PATTERN_DOUBLE_SPIRAL 7
#define PATTERN_ROTATING_PONG 8
#define PATTERN_LASERS 9
#define PATTERN_LASERS_ALL_AT_ONCE 10
#define PATTERN_RAINFALL 11
#define PATTERN_BASKET_WEAVING 12
#define PATTERN_COMET_TRAILS 13
#define PATTERN_ROTATING_HEXAGONS 14
#define PATTERN_COUNTER_ROTATING_HEXAGONS 15
#define PATTERN_VARIABLE_SPEED_ROTATION 16
#define PATTERN_FLASHING_HEXAGONS 17
#define PATTERN_FLASHING_HEXAGONS_WARP 18
#define NUM_PATTERNS 19

#define DEFAULT_SPEED 3
#define DEFAULT_OVERLAYSPEED 8
#define DEFAULT_OVERLAYWIDTH 5
#define DEFAULT_OVERLAYDENSITY 3
#define DEFAULT_DENSITY 4
#define DEFAULT_WIDTH 5
#define DEFAULT_REVERSE false
#define DEFAULT_AUTOCYCLEPALETTES true
#define DEFAULT_SECONDSPERPALETTE 10

//char macAddressM1[] = "4C:75:25:64:E5:DC"; // (receiver on prototype)
//char macAddressM2[] = "A8:42:E3:AB:A6:88"; // (2nd receiver on prototype) - cant find this one?
//char macAddressM3[] = "C8:F0:9E:9F:36:A8"; // (sender)
//char macAddressM4[] = "A8:42:E3:48:A1:8C"; // (2nd receiver on prototype)
//
//char macAddressB1[] = "00:00:00:00:00:00"; // (receiver on prototype)
//char macAddressB3[] = "FC:B4:67:74:5C:68"; // (sender)

uint8_t receiverAddress1[] = {0x4C, 0x75, 0x25, 0x64, 0xE5, 0xDC};
uint8_t receiverAddress2[] = {0xA8, 0x42, 0xE3, 0x48, 0xA1, 0x8C};
uint8_t senderAddress[] = {0xC8, 0xF0, 0x9E, 0x9F, 0x36, 0xA8};
char receiverMacAddress1[] = "4C:75:25:64:E5:DC";
char receiverMacAddress2[] = "A8:42:E3:48:A1:8C";
char senderMacAddress[] = "C8:F0:9E:9F:36:A8";

typedef struct msg {
  uint8_t action;
  int value;
} msg;

esp_err_t send(msg m) { return esp_now_send(0, (uint8_t *)&m, sizeof(msg)); }
