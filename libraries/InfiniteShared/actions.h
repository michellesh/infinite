// See README for reference
// https://github.com/michellesh/infinite

String mp3url = "https://github.com/michellesh/infinite/raw/main/songs/cornfield-chase.mp3";

Action actions[] = {
    time().track("cornfield-chase.mp3").bpm(31).pattern(PATTERN_TWINKLE).speed(4).width(1).density(1).palette(PALETTE_ORANGEPURPLEBLUE),
    time(4800).patternFade(PATTERN_COMET_TRAILS, 500).bpm(31).width(7).palette(PALETTE_FIRE),
    time(18200).pattern(PATTERN_RAINFALL_FALL_ON_BEAT).bpm(34).width(6).palette(PALETTE_ORANGEPURPLEBLUE),
    time(33200).pattern(PATTERN_BASKET_WEAVING).bpm(96).width(1),
    time(42200).pattern(PATTERN_LASERS).width(10).palette(PALETTE_BLUESANDPURPLES),
    time(53500).pattern(PATTERN_ROTATING_PONG).width(1).density(10).bpm(96),
    time(65000).pattern(PATTERN_LASERS).width(6).bpm(192),
    time(68800).pattern(PATTERN_ROTATING_HEXAGONS).bpm(48).density(9).width(4).palette(PALETTE_BLUEPURPLEREDORANGE),
    time(72400).width(3),
    time(76200).width(2),
    time(80100).width(1),
    time(83500).pattern(PATTERN_ROTATING_HEXAGONS).bpm(48).density(3).width(9).palette(PALETTE_BLUESANDPURPLES),
    time(87300).bpm(96).density(5),
    time(91300).bpm(144).density(7),
    time(94800).bpm(189).density(9),
    time(99000).pattern(PATTERN_SOLID_OVERLAY_RAILGUN).bpm(48).palette(PALETTE_FIRE),
    time(102900).bpm(96),
    time(106000).reverse(1),
    time(111000).pattern(PATTERN_FLASHING_HEXAGONS_WARP),
    time(113500).pattern(PATTERN_TWINKLE).width(1).speed(7).density(3).overlay(OVERLAY_BRIGHTNESS_SINWAVE).bpm(96).palette(PALETTE_PINKCLOUDUPDATE),
    time(119400).pattern(PATTERN_TWINKLE).width(1).speed(3).density(1).overlay(0)
};

Fade fades[] = {
    fadeOut(5000, 10000),
    fadeIn(10000, 15000),
};
