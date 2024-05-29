# infinite

Tubey McTubeface

## Actions reference

### Example

See `libraries/InfiniteShared/actions.h`

```cpp
Action actions[] = {
    time().track(5).bpm(112).pattern(PATTERN_SOLID_OVERLAY),
    time(5000).bpm(224).pattern(PATTERN_LASERS_DOUBLES),
    time(10000).pattern(PATTERN_LASERS),
    time(15000).pattern(PATTERN_ROTATING_HEXAGONS).speed(9.5).width(10),
    time(20000).speed(1),
    time(25000).width(1).speed(6).density(1),
    time(30000)
        .pattern(PATTERN_TWINKLE)
        .width(5)
        .density(4)
        .speed(7)
        .overlay(OVERLAY_BRIGHTNESS_SINWAVE)
        .bpm(112)};
};

Fade fades[] = {
    fadeOut(5000, 10000),
    fadeIn(10000, 15000),
};
```

1. Every line must start with `time(milliseconds)`. After, commands can be chained in any order. All commands chained to one `time` command will be executed at that same timestamp.
2. Each subsequent action retains the state of previous action.
3. There must be at least 350 milliseconds between each action. This is because there needs to be a delay between sending to each of the 8 receivers. The delay is currently set to 50ms per receiver.
4. Actions must be in order - each subsequent timestamp must be greater than the previous.
5. Negative values are untested. Use `reverse`.

### Action functions

| Function | Description |
| -------- | ----------- |
| `.time(milliseconds)` | Create an action at this timestamp. `milliseconds` cannot be 0, use `.time()` for first action. |
| `.track(int trackNumber)` | Set the track. Should be in same order as SD card. For first track use `.track(1)` (no zeros). Has no effect if running sequence from web server. |
| `.pattern(int patternId)`  | Set the pattern. See spreadsheet for current pattern variables. |
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

### Fade functions

These functions have no effect if running sequence from web server.

| Function | Description |
| -------- | ----------- |
| `.fadeOut(startMillis, endMillis)` | Fade volume out starting at `startMillis` reaching volume 0 by `endMillis` |
| `.fadeIn(startMillis, endMillis)` | Fade volume in starting at `startMillis` reaching max volume by `endMillis` |
