# infinite

Tubey McTubeface

## Actions reference

See file `sender/actions.h`

### Example

```cpp
Action actions[] = {
  time(3000).track(5).pattern(PATTERN_SOLID_OVERLAY).overlaySpeed(9.5),
  time(8000).pattern(PATTERN_ROTATING_HEXAGONS).speed(9.5).width(10),
  time(12000).speed(1),
  time(15000).width(1).speed(6),
};

Fade fades[] = {
  fadeOut(5000, 10000),
  fadeIn(10000, 15000),
};
```

1. Every line must start with `time(milliseconds)`. After, commands can be chained in any order. All commands chained to one `time` command will be executed at that same timestamp.
2. There must be at least 350 milliseconds between each action. This is because there needs to be a delay between sending to each of the 8 receivers. The delay is currently set to 50ms.
3. Each subsequent timestamp must be greater than the previous.
4. Negative values are untested. Use `reverse`.

### Action functions

| Function | Description |
| -------- | ----------- |
| `.time(milliseconds)` | Create an action at this timestamp. |
| `.track(int trackNumber)` | Set the track. Should be in same order as SD card. For first track use `.track(1)` (no zeros) |
| `.pattern(int patternId)`  | Set the pattern<br><br>`PATTERN_SOLID_OVERLAY`<br>`PATTERN_SOLID_OVERLAY_RAILGUN`<br>`PATTERN_TWINKLE`<br>`PATTERN_RANDOM_FADING_SEGMENTS`<br>`PATTERN_RANDOM_FLASHING_SEGMENTS`<br>`PATTERN_TWINKLE_OVERLAY`<br>`PATTERN_SINGLE_SPIRAL`<br>`PATTERN_DOUBLE_SPIRAL`<br>`PATTERN_ROTATING_PONG`<br>`PATTERN_LASERS`<br>`PATTERN_LASERS_ALL_AT_ONCE`<br>`PATTERN_RAINFALL`<br>`PATTERN_BASKET_WEAVING`<br>`PATTERN_COMET_TRAILS`<br>`PATTERN_ROTATING_HEXAGONS`<br>`PATTERN_COUNTER_ROTATING_HEXAGONS`<br>`PATTERN_VARIABLE_SPEED_ROTATION`<br>`PATTERN_FLASHING_HEXAGONS`<br>`PATTERN_FLASHING_HEXAGONS_WARP` |
| `.palette(int paletteId)` | Set the color palette<br><br>`PALETTE_FIRE`<br>`PALETTE_OCEAN`<br>`PALETTE_FLORAL`<br>`PALETTE_ICE`<br>`PALETTE_FAIRY` |
| `.colorMode(int colorModeId)` | Set the color mode<br><br>`COLOR_MODE_SOLID`<br>`COLOR_MODE_INDEX_GRADIENT`<br>`COLOR_MODE_DEPTH_GRADIENT`<br>`COLOR_MODE_ANGLE_GRADIENT` |
| `.speed(float value)` | Set the speed 1-10 |
| `.density(float value)` | Set the density 1-10 |
| `.width(float value)` | Set the width 1-10 |
| `.overlaySpeed(float value)` | Set the overlay speed 1-10. Affects the overlay patterns:<br><br>`PATTERN_SOLID_OVERLAY`<br>`PATTERN_SOLID_OVERLAY_RAILGUN` |
| `.overlayDensity(float value)` | Set the overlay density 1-10. Affects the overlay patterns:<br><br>`PATTERN_SOLID_OVERLAY`<br>`PATTERN_SOLID_OVERLAY_RAILGUN` |
| `.overlayWidth(float value)` | Set the overlay width 1-10. Affects the overlay patterns:<br><br>`PATTERN_SOLID_OVERLAY`<br>`PATTERN_SOLID_OVERLAY_RAILGUN` |
| `.reverse(bool isReversed)` | Set reverse to true or false. Defaults to false. |

### Fade functions

| Function | Description |
| -------- | ----------- |
| `.fadeOut(startMillis, endMillis)` | Fade volume out starting at `startMillis` reaching volume 0 by `endMillis` |
| `.fadeIn(startMillis, endMillis)` | Fade volume in starting at `startMillis` reaching max volume by `endMillis` |
