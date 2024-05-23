// See README for reference
//
// https://github.com/michellesh/infinite

Action actions[] = {
    time(1000).track(5),
    time(3000).track(5).pattern(PATTERN_SOLID_OVERLAY).overlaySpeed(9.5),
    time(8000).pattern(PATTERN_ROTATING_HEXAGONS).speed(9.5).width(10),
    time(12000).speed(1),
    time(15000).width(1).speed(6),
};

Fade fades[] = {
  fadeOut(5000, 10000),
  fadeIn(10000, 15000),
};

// Queen - BPM match
// track(5).pattern(PATTERN_ROTATING_HEXAGONS).speed(9.5),

