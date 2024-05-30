DEFINE_GRADIENT_PALETTE(_firePalette){
    0,   139, 10,  0,  // dark red with hint of orange
    255, 255, 150, 0}; // gold with hint of orange
CRGBPalette16 firePalette = _firePalette;

DEFINE_GRADIENT_PALETTE(_oceanPalette){
    0,   94, 255, 94,  // CHSV(HUE_GREEN, 100, 255)
    255, 15, 150, 15}; // greenish teal
CRGBPalette16 oceanPalette = _oceanPalette;

DEFINE_GRADIENT_PALETTE(_floralPalette){0,   255, 127, 80,   // CRGB::Coral
                                        255, 230, 230, 250}; // CRGB::Lavender
CRGBPalette16 floralPalette = _floralPalette;

DEFINE_GRADIENT_PALETTE(_icePalette){0,   224, 240, 255,  // light blue
                                     255, 31,  147, 255}; // medium blue
CRGBPalette16 icePalette = _icePalette;

DEFINE_GRADIENT_PALETTE(_fairyPalette){0,   63,  57,  11,  // "QuarterFairy"
                                       255, 255, 227, 45}; // CRGB::FairyLight
CRGBPalette16 fairyPalette = _fairyPalette;

uint8_t percent(uint8_t percent) { return map(percent, 0, 100, 0, 255); }

DEFINE_GRADIENT_PALETTE(_orangePurpleBlue){
    percent(0),  239, 146, 58,  percent(12),  159, 85,  137,
    percent(24), 93,  47,  104, percent(38),  59,  106, 165,
    percent(45), 6,   53,  109, percent(55),  6,   53,  109,
    percent(62), 59,  106, 165, percent(76),  93,  47,  104,
    percent(88), 159, 85,  137, percent(100), 239, 146, 58};
CRGBPalette16 orangePurpleBlue = _orangePurpleBlue;

DEFINE_GRADIENT_PALETTE(_pinkPurplePink){
    percent(0),   220, 144, 182, percent(11), 186, 102, 151,
    percent(20),  157, 80,  150, percent(50), 46,  26,  113,
    percent(80),  157, 80,  150, percent(90), 186, 102, 151,
    percent(100), 220, 144, 182};
CRGBPalette16 pinkPurplePink = _pinkPurplePink;

DEFINE_GRADIENT_PALETTE(_fireAndIce){
    percent(0),  164, 29,  10,  percent(20),  235, 71,  15,
    percent(40), 250, 103, 29,  percent(60),  88,  139, 194,
    percent(80), 42,  82,  152, percent(100), 21,  31,  52};
CRGBPalette16 fireAndIce = _fireAndIce;

DEFINE_GRADIENT_PALETTE(_pinkCloud){percent(0),   252, 200, 169,
                                    percent(50),  236, 100, 153,
                                    percent(100), 128, 150, 226};
CRGBPalette16 pinkCloud = _pinkCloud;

DEFINE_GRADIENT_PALETTE(_yellowGreen){
    percent(0),   220, 243, 134, percent(29), 202, 228, 128,
    percent(55),  132, 216, 119, percent(80), 64,  158, 53,
    percent(100), 0,   88,  0};
CRGBPalette16 yellowGreen = _yellowGreen;

DEFINE_GRADIENT_PALETTE(_bluesAndPurples){
    percent(0),  123, 145, 204, percent(12),  65,  79,  134,
    percent(22), 158, 111, 245, percent(34),  142, 121, 223,
    percent(44), 86,  191, 253, percent(55),  194, 183, 237,
    percent(67), 82,  128, 244, percent(78),  193, 137, 244,
    percent(90), 65,  79,  134, percent(100), 123, 145, 204};
CRGBPalette16 bluesAndPurples = _bluesAndPurples;

DEFINE_GRADIENT_PALETTE(_rainbow){
    percent(0),  195, 8,   202, percent(10),  219, 9,   103,
    percent(19), 243, 56,  19,  percent(28),  248, 193, 26,
    percent(37), 139, 214, 8,   percent(46),  20,  229, 9,
    percent(55), 14,  234, 138, percent(64),  8,   184, 216,
    percent(73), 8,   55,  206, percent(82),  79,  9,   207,
    percent(91), 79,  9,   207, percent(100), 219, 9,   227};
CRGBPalette16 rainbow = _rainbow;

DEFINE_GRADIENT_PALETTE(_bluePurpleRedOrange){
    percent(0),  59,  57,  164, percent(18),  122, 88,  183,
    percent(32), 89,  45,  142, percent(44),  104, 28,  93,
    percent(59), 118, 23,  43,  percent(78),  221, 106, 62,
    percent(93), 236, 170, 86,  percent(100), 249, 234, 211};
CRGBPalette16 bluePurpleRedOrange = _bluePurpleRedOrange;

// clang-format off
CRGBPalette16* activePalettes[] = {
  &orangePurpleBlue,
  &pinkPurplePink,
  &fireAndIce,
  &pinkCloud,
  &yellowGreen,
  &bluesAndPurples,
  &rainbow,
  &bluePurpleRedOrange,
  &firePalette,
  &oceanPalette,
  &floralPalette,
  &icePalette,
  &fairyPalette
  // TODO update NUM_PALETTES and palette IDs!
};
// clang-format on
