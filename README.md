# infinite

Tubey McTubeface

## Actions reference

### Example

In file: `libraries/InfiniteShared/actions.h`

```cpp
String mp3url = "https://github.com/michellesh/infinite/raw/main/songs/cornfield-chase.mp3";

Action actions[] = {
    time().bpm(31).pattern(PATTERN_TWINKLE).speed(4).width(1).density(1),
    time(4800).patternFade(PATTERN_COMET_TRAILS, 500).bpm(31).width(7),
    time(18200).pattern(PATTERN_RAINFALL_FALL_ON_BEAT).bpm(34).width(6),
    time(33200).pattern(PATTERN_BASKET_WEAVING).bpm(96).width(1),
    time(42200).pattern(PATTERN_LASERS).width(10),
    time(53525).pattern(PATTERN_LASERS).width(2).bpm(189),
    time(68800).pattern(PATTERN_ROTATING_HEXAGONS).bpm(48).density(9).width(4),
    time(72400).width(3),
    time(76200).width(2),
    time(80100).width(1),
    time(83500).pattern(PATTERN_ROTATING_HEXAGONS).bpm(48).density(3).width(9),
    time(87300).bpm(96).density(5),
    time(91300).bpm(144).density(7),
    time(94800).bpm(189).density(9),
    time(99000).pattern(PATTERN_SOLID_OVERLAY_RAILGUN).bpm(48),
    time(102900).bpm(96),
    time(106000).reverse(1),
    time(111000).pattern(PATTERN_FLASHING_HEXAGONS_WARP),
    time(113500).pattern(PATTERN_TWINKLE).width(1).speed(7).density(3).overlay(OVERLAY_BRIGHTNESS_SINWAVE).bpm(96),
    time(119400).pattern(PATTERN_TWINKLE).width(1).speed(3).density(1).overlay(0)
};
```

1. Every line must start with `time(milliseconds)`. After, commands can be chained in any order. All commands chained to one `time` command will be executed at that same timestamp.
2. Each subsequent action retains the state of previous action.
3. There must be at least 350 milliseconds between each action. This is because there needs to be a delay between sending to each of the 8 receivers. The delay is currently set to 50ms per receiver.
4. Actions must be in order - each subsequent timestamp must be greater than the previous.
5. Negative values are untested. Use `reverse`.

### Variables

| Variable | Description |
| -------- | ----------- |
| `mp3url` | The URL of the mp3 file. In the web server, this audio will play from the browser when the "Play Sequence" button is clicked. Include this at the top of the `actions.h` file. Example:<br></br>```String mp3url = "https://github.com/michellesh/infinite/raw/main/songs/cornfield-chase.mp3";``` |


### Action functions

| Function | Description |
| -------- | ----------- |
| `.time(milliseconds)` | Create an action at this timestamp. `milliseconds` cannot be 0, use `.time()` for first action. |
| `.track(int trackNumber, delay = 0)` | Set the track. Should be in same order as SD card. For first track use `.track(0)` (zero-indexed). This function has no effect if running sequence from web server (use `mp3url` instead).<br><br>`delay` (optional) values greater than 0 will delay playing the track that many milliseconds. Values less than 0 will delay firing the actions after that many milliseconds. If omitted, neither the track playing nor the actions firing will be delayed. -300 worked well for me in X1_RECEIVER_MODE, it might end up being the same value for every song. |
| `.pattern(int patternId)`  | Set the pattern. See spreadsheet for current pattern variables. |
| `.patternFade(int patternId, int fadeMillis = 2000)`  | Set the pattern, and fade the current pattern out and the new pattern in. The second function argument (optional) is how many milliseconds to fade out and to fade in (each). If omitted, default is 2000ms. |
| `.palette(int paletteId)` | Set the color palette<br><br>`PALETTE_FIRE`<br>`PALETTE_OCEAN`<br>`PALETTE_FLORAL`<br>`PALETTE_ICE`<br>`PALETTE_FAIRY` |
| `.colorMode(int colorModeId)` | Set the color mode<br><br>`COLOR_MODE_SOLID`<br>`COLOR_MODE_INDEX_GRADIENT`<br>`COLOR_MODE_DEPTH_GRADIENT`<br>`COLOR_MODE_ANGLE_GRADIENT` |
| `.bpm(int value)` | Set the bpm |
| `.speed(float value)` | Set the speed 1-10 |
| `.density(float value)` | Set the density 1-10 |
| `.width(float value)` | Set the width 1-10 |
| `.overlaySpeed(float value)` | Set the overlay speed 1-10. |
| `.overlayDensity(float value)` | Set the overlay density 1-10. |
| `.overlayWidth(float value)` | Set the overlay width 1-10. |
| `.reverse(bool isReversed)` | Set reverse to true or false. Defaults to false. |
| `.overlay(int value)` | Set an overlay pattern to play over the current pattern.<br><br>`OVERLAY_BRIGHTNESS_SINWAVE`<br>`OVERLAY_BRIGHTNESS_STROBE` |

### Volume fade functions

Fade the volume of the song. These functions have no effect if running the sequence from the web server.

| Function | Description |
| -------- | ----------- |
| `.fadeOut(startMillis, endMillis)` | Fade volume out starting at `startMillis` reaching volume 0 by `endMillis` |
| `.fadeIn(startMillis, endMillis)` | Fade volume in starting at `startMillis` reaching max volume by `endMillis` |


## SD card/sender setup

Steps to set up the prototype as a receiver and set up the sender to play music from the SD card and send choreography to receiver.

1. Change prototype ESP32 from "single board mode" to "single receiver mode"

   - Open file `libraries/InfiniteShared/InfiniteShared.h`
   - Near the top of the file, change the line `#define MODE SINGLE_BOARD_MODE` to `#define MODE X1_RECEIVER_MODE`. (The web server only works in `SINGLE_BOARD_MODE` though, so swap this back if you want to use the web server.)
   - On the next line down, change the mac address `char receiver1[] = "4C:75:25:64:E5:DC";` to your prototype ESP32's mac address. To find the MAC address of your ESP32, upload [this code](https://github.com/michellesh/test-leds/blob/main/mac-address/mac-address.ino) to the board, and the MAC address will print to the serial monitor.
   - Upload `prototype/prototype.ino` sketch to the receiver board.

2. Add mp3 files to your SD card in the root directory

3. Upload code to sender

   - Open file `sender/sender.ino` as new sketch in Arduino IDE.
   - Upload `sender/sender.ino` sketch to the sender board.
   - Open Serial Monitor. It should print a list of tracks and their track numbers, which are the numbers you'll use when you call the `.track()` function in the `actions.h` file.

4. Set up choreography in the same way as before by changing the `actions.h` file, but instead of testing changes by uploading to the prototype's ESP32, upload the `sender/sender.ino` code to the sender.
