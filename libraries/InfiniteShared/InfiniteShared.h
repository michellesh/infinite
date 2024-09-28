#define SINGLE_BOARD_MODE 0 // no sender, single board controlled by web server
#define X1_RECEIVER_MODE 1  // sender and single receiver
#define X8_RECEIVER_MODE 2  // sender and two receivers

#define MODE X1_RECEIVER_MODE

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
#define ACTION_SET_OVERLAY 13

#define PATTERN_SOLID 0
#define PATTERN_SOLID_OVERLAY 1
#define PATTERN_SOLID_OVERLAY_RAILGUN 2
#define PATTERN_TWINKLE 3
#define PATTERN_RANDOM_FADING_SEGMENTS 4
#define PATTERN_RANDOM_FLASHING_SEGMENTS 5
#define PATTERN_RANDOM_FLASHING_SEGMENTS_GLITCH 6
#define PATTERN_TWINKLE_OVERLAY 7
#define PATTERN_ROTATING_PONG 8
#define PATTERN_LASERS 9
#define PATTERN_LASERS_DOUBLES 10
#define PATTERN_LASERS_ALL_AT_ONCE 11
#define PATTERN_RAINFALL_CYCLE_ON_BEAT 12
#define PATTERN_RAINFALL_FALL_ON_BEAT 13
#define PATTERN_RAINFALL_COMET_TRAILS 14
#define PATTERN_BASKET_WEAVING 15
#define PATTERN_COMET_TRAILS 16
#define PATTERN_POWER_UP_AND_FIRE 17
#define PATTERN_POWER_UP_AND_FIRE_BROKEN 18
#define PATTERN_SNAKES 19
#define PATTERN_ROTATING_HEXAGONS 20
#define PATTERN_COUNTER_ROTATING_HEXAGONS 21
#define PATTERN_VARIABLE_SPEED_ROTATION 22
#define PATTERN_VARIABLE_SPEED_ROTATION_END 23
#define PATTERN_FLASHING_HEXAGONS 24
#define PATTERN_FLASHING_HEXAGONS_WARP 25
#define PATTERN_WAVEFORM_BOUNCING_SINGLE 26
#define PATTERN_WAVEFORM_BOUNCING_DOUBLE 27
#define PATTERN_WAVEFORM_BOUNCING_NODES 28
#define NUM_PATTERNS 29

#define OVERLAY_BRIGHTNESS_SINWAVE 1
#define OVERLAY_BRIGHTNESS_STROBE 2

#define DEFAULT_PATTERN PATTERN_TWINKLE
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
#define DEFAULT_FADE_MILLIS 2000

#define PALETTE_ORANGEPURPLEBLUE 0
#define PALETTE_FIREANDICE 1
#define PALETTE_BLUESANDPURPLES 2
#define PALETTE_RAINBOW 3
#define PALETTE_BLUEPURPLEREDORANGE 4
#define PALETTE_FIRE 5
#define PALETTE_OCEAN 6
#define PALETTE_PINKBLUEPINK 7
#define PALETTE_BLUETEALBLUE 8
#define PALETTE_BLUEMAGENTAPURPLE 9
#define PALETTE_MINTMAGENTAPURPLE 10
#define PALETTE_ORANGEPINKMINT 11
#define PALETTE_PEACEFULBLUE 12
#define PALETTE_BLUETEALBLUE2 13
#define PALETTE_BLUEPURPLEPINK 14
#define PALETTE_PINKWHITEORANGE 15
#define PALETTE_YELLOWGREEN 16
#define PALETTE_PINKCLOUDUPDATE 17
#define PALETTE_ALLBLACK 18
#define NUM_PALETTES 19

#define COLOR_MODE_SOLID 0
#define COLOR_MODE_INDEX_GRADIENT 1
#define COLOR_MODE_DEPTH_GRADIENT 2
#define COLOR_MODE_ANGLE_GRADIENT 3
#define COLOR_MODE_CUSTOM_GRADIENT 4

// **OLD M2** char macAddressM2[] = "A8:42:E3:AB:A6:88"; // cant find this one?
// char receiverB1[] = "00:00:00:00:00:00"; // (receiver on prototype)
// char senderB3[] = "FC:B4:67:74:5C:68"; // (sender)
// char senderMS[] = "C8:F0:9E:9F:36:A8"; // (sender)

//char receiverM1[] = "4C:75:25:64:E5:DC";
//char receiverM2[] = "A8:42:E3:48:A1:8C";
char receiverM3[] = "A0:B7:65:63:7D:8C";
char receiverM4[] = "48:E7:29:C9:F9:F8";
//char receiverM5[] = "FC:B4:67:74:46:04";
char receiverM6[] = "FC:B4:67:74:5A:38";
char receiverM7[] = "48:E7:29:C9:74:7C";
char receiverM8[] = "EC:62:60:32:CC:78";
char receiverM9[] = "D0:EF:76:47:5E:7C";
char receiverM11[] = "D0:EF:76:47:74:48";
char receiverM12[] = "D0:EF:76:47:34:A4";
//char *receivers[] = {receiverM1, receiverM2, receiverM3, receiverM4,
//                     receiverM5, receiverM6, receiverM7, receiverM8};
char *receivers[] = {receiverM6, receiverM11, receiverM9, receiverM3,
                     receiverM12, receiverM4, receiverM8, receiverM7};

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
  int overlay = 0;
  unsigned long fadeMillis = DEFAULT_FADE_MILLIS;
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
