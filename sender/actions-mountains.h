Action mountainsActions[] = {
    time().track("moutains.mp3").bpm(48).pattern(PATTERN_TWINKLE).speed(4).width(1).density(1).palette(PALETTE_PEACEFULBLUE),
    time(1750).speed(1),
    time(3000).speed(4),
    time(4250).speed(1),
    time(5500).speed(4),
    time(6750).speed(1),
    time(8000).speed(4),
    time(9250).speed(1),
    time(10500).speed(4),
    time(11750).speed(1),
    time(13000).speed(4),
    time(14250).speed(1),
    time(15500).speed(4),
    time(16750).speed(1),
    time(18000).speed(4),
    time(19250).patternFade(PATTERN_SNAKES, 1250).palette(PALETTE_BLUETEALBLUE),
    time(30500).pattern(PATTERN_ROTATING_HEXAGONS).bpm(24).density(1).width(3),
    time(40500).pattern(PATTERN_VARIABLE_SPEED_ROTATION_END),
    time(49750).patternFade(PATTERN_ROTATING_PONG, 1250).bpm(48),

    time(59900).patternFade(PATTERN_RANDOM_FADING_SEGMENTS, 1250).speed(2).density(10).width(10).palette(PALETTE_FIRE),

    time(68000).palette(PALETTE_BLUETEALBLUE),
    time(70500).palette(PALETTE_FIRE),
    time(73000).palette(PALETTE_BLUETEALBLUE),
    time(75500).palette(PALETTE_FIRE),

    time(78000).pattern(PATTERN_COMET_TRAILS).bpm(100).palette(PALETTE_FIREANDICE).colorMode(COLOR_MODE_ANGLE_GRADIENT),
    time(82900).reverse(1),
    time(85400).reverse(0).speed(6).bpm(200),
    time(90400).reverse(1),
    time(92900).reverse(0).speed(10).bpm(300),

    time(95500).pattern(PATTERN_TWINKLE).bpm(100).density(1).width(1).speed(5).overlay(OVERLAY_BRIGHTNESS_SINWAVE),

    time(105100).pattern(PATTERN_ROTATING_HEXAGONS).overlay(0).bpm(34).density(1).width(8).palette(PALETTE_FIRE),
    time(114100).bpm(61),
    time(116250).bpm(116),
    time(118400).bpm(179),
    time(120500).bpm(237),
    time(122150).pattern(PATTERN_VARIABLE_SPEED_ROTATION_END),

    time(138500).pattern(PATTERN_POWER_UP_AND_FIRE_BROKEN).bpm(60),

    time(146500).pattern(PATTERN_RAINFALL_COMET_TRAILS).bpm(120).width(10).palette(PALETTE_FIREANDICE).colorMode(COLOR_MODE_ANGLE_GRADIENT),

    time(154500).pattern(PATTERN_TWINKLE).speed(6).density(5).width(1).palette(PALETTE_BLUESANDPURPLES),

    time(158700).overlay(OVERLAY_BRIGHTNESS_STROBE).bpm(60),
    time(162500).palette(PALETTE_BLUETEALBLUE),
    time(163500).palette(PALETTE_FIRE),
    time(164500).palette(PALETTE_BLUETEALBLUE),
    time(165500).palette(PALETTE_FIRE),

    time(166500).pattern(PATTERN_SNAKES).overlay(0).bpm(120).colorMode(COLOR_MODE_DEPTH_GRADIENT),

    time(173700).pattern(PATTERN_SOLID_OVERLAY).bpm(15).overlayWidth(10).overlayDensity(1).palette(PALETTE_PINKCLOUDUPDATE).colorMode(COLOR_MODE_ANGLE_GRADIENT),
    time(178000).palette(PALETTE_BLUEPURPLEREDORANGE),
    time(182000).palette(PALETTE_BLUETEALBLUE),
    time(186000).palette(PALETTE_FIRE),

    time(189500).palette(PALETTE_ALLBLACK), // prevent last partial wave
    time(190500).pattern(PATTERN_RANDOM_FLASHING_SEGMENTS).overlay(0).speed(5).density(10).palette(PALETTE_PINKCLOUDUPDATE).colorMode(COLOR_MODE_DEPTH_GRADIENT),
    time(192500).palette(PALETTE_ALLBLACK),

    time(194500).palette(PALETTE_BLUEPURPLEREDORANGE),
    time(196500).palette(PALETTE_ALLBLACK),

    time(198500).palette(PALETTE_BLUETEALBLUE),
    time(200500).palette(PALETTE_ALLBLACK),

    time(202500).palette(PALETTE_FIRE),
    time(204500).palette(PALETTE_ALLBLACK),

    time(206500).pattern(PATTERN_TWINKLE).speed(4).width(1).density(1).palette(PALETTE_PEACEFULBLUE),
};
