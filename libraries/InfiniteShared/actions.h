// See README for reference
// https://github.com/michellesh/infinite

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

Fade fades[] = {
    fadeOut(5000, 10000),
    fadeIn(10000, 15000),
};
