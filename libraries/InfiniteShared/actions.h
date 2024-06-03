// See README for reference
// https://github.com/michellesh/infinite

Action actions[] = {
    time().track(5).bpm(112).pattern(PATTERN_ROTATING_HEXAGONS),
    time(5000).patternFade(PATTERN_TWINKLE),
    time(10000).patternFade(PATTERN_ROTATING_HEXAGONS).bpm(224).width(10),
    time(15000).patternFade(PATTERN_TWINKLE),
    time(20000).patternFade(PATTERN_ROTATING_HEXAGONS),
};

Fade fades[] = {
    fadeOut(5000, 10000),
    fadeIn(10000, 15000),
};
