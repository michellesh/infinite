#define SINGLE_BOARD_MODE 0 // no sender, single board controlled by web server
#define X1_RECEIVER_MODE 1  // sender and single receiver
#define X8_RECEIVER_MODE 2  // sender and two receivers

#define MODE SINGLE_BOARD_MODE

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
#define ACTION_SET_BPM 12

#define PATTERN_SOLID 0
#define PATTERN_SOLID_OVERLAY 1
#define PATTERN_SOLID_OVERLAY_RAILGUN 2
#define PATTERN_TWINKLE 3
#define PATTERN_RANDOM_FADING_SEGMENTS 4
#define PATTERN_RANDOM_FLASHING_SEGMENTS 5
#define PATTERN_TWINKLE_OVERLAY 6
#define PATTERN_SINGLE_SPIRAL 7
#define PATTERN_DOUBLE_SPIRAL 8
#define PATTERN_ROTATING_PONG 9
#define PATTERN_LASERS 10
#define PATTERN_LASERS_DOUBLES 11
#define PATTERN_LASERS_ALL_AT_ONCE 12
#define PATTERN_RAINFALL 13
#define PATTERN_BASKET_WEAVING 14
#define PATTERN_COMET_TRAILS 15
#define PATTERN_ROTATING_HEXAGONS 16
#define PATTERN_COUNTER_ROTATING_HEXAGONS 17
#define PATTERN_VARIABLE_SPEED_ROTATION 18
#define PATTERN_VARIABLE_SPEED_ROTATION_END 19
#define PATTERN_FLASHING_HEXAGONS 20
#define PATTERN_FLASHING_HEXAGONS_WARP 21
#define NUM_PATTERNS 22

#define DEFAULT_PATTERN PATTERN_COUNTER_ROTATING_HEXAGONS
#define DEFAULT_PALETTE 0
#define DEFAULT_COLORMODE 2
#define DEFAULT_SPEED 3
#define DEFAULT_OVERLAYSPEED 8
#define DEFAULT_OVERLAYWIDTH 5
#define DEFAULT_OVERLAYDENSITY 3
#define DEFAULT_DENSITY 4
#define DEFAULT_WIDTH 5
#define DEFAULT_REVERSE false
#define DEFAULT_AUTOCYCLEPALETTES false
#define DEFAULT_SECONDSPERPALETTE 10
#define DEFAULT_BPM 112

#define PALETTE_FIRE 0
#define PALETTE_OCEAN 1
#define PALETTE_FLORAL 2
#define PALETTE_ICE 3
#define PALETTE_FAIRY 4
#define NUM_PALETTES 5

#define COLOR_MODE_SOLID 0
#define COLOR_MODE_INDEX_GRADIENT 1
#define COLOR_MODE_DEPTH_GRADIENT 2
#define COLOR_MODE_ANGLE_GRADIENT 3
#define COLOR_MODE_CUSTOM_GRADIENT 4

// **OLD M2** char macAddressM2[] = "A8:42:E3:AB:A6:88"; // cant find this one?
// char receiverB1[] = "00:00:00:00:00:00"; // (receiver on prototype)
// char senderB3[] = "FC:B4:67:74:5C:68"; // (sender)
// char senderMS[] = "C8:F0:9E:9F:36:A8"; // (sender)

char receiverM1[] = "4C:75:25:64:E5:DC";
char receiverM2[] = "A8:42:E3:48:A1:8C";
char receiverM3[] = "A0:B7:65:63:7D:8C";
char receiverM4[] = "48:E7:29:C9:F9:F8";
char receiverM5[] = "FC:B4:67:74:46:04";
char receiverM6[] = "FC:B4:67:74:5A:38";
char receiverM7[] = "48:E7:29:C9:74:7C";
char receiverM8[] = "EC:62:60:32:CC:78";
char *receivers[] = {receiverM1, receiverM2, receiverM3, receiverM4,
                     receiverM5, receiverM6, receiverM7, receiverM8};

typedef struct msg {
  unsigned long delay = 0;
  int pattern = DEFAULT_PATTERN;
  int palette = DEFAULT_PALETTE;
  int colorMode = DEFAULT_COLORMODE;
  float speed = DEFAULT_SPEED;
  float density = DEFAULT_DENSITY;
  float width = DEFAULT_WIDTH;
  float overlaySpeed = DEFAULT_OVERLAYSPEED;
  float overlayDensity = DEFAULT_OVERLAYDENSITY;
  float overlayWidth = DEFAULT_OVERLAYWIDTH;
  bool reverse = DEFAULT_REVERSE;
  int bpm = DEFAULT_BPM;
} msg;

esp_err_t send(msg m) { return esp_now_send(0, (uint8_t *)&m, sizeof(msg)); }

// Choreography stuff
#define MAX_VOLUME 21

msg data;

#include "Action.h"
#include "Fade.h"

Action time(unsigned long timestamp = 0) {
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

#include "actions.h"

int numActions = sizeof(actions) / sizeof(actions[0]);
int numFades = sizeof(fades) / sizeof(fades[0]);
