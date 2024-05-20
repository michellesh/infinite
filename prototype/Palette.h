#define MAX_PALETTE_INDEX 245

class Palette {
private:
  CRGBPalette16 _currentPalette = *(activePalettes[0]);
  CRGBPalette16 _targetPalette = *(activePalettes[0]);
  uint8_t _activeColorMode = DEPTH_GRADIENT;
  Timer _paletteCycleTimer = {DEFAULT_SECONDSPERPALETTE * 1000};

  void _setNextColorPalette() {
    const uint8_t numberOfPalettes =
        sizeof(activePalettes) / sizeof(activePalettes[0]);
    static uint8_t whichPalette = -1;
    whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

    _targetPalette = *(activePalettes[whichPalette]);
  }

public:
  static const uint8_t SOLID = 0;
  static const uint8_t INDEX_GRADIENT = 1;
  static const uint8_t DEPTH_GRADIENT = 2;
  static const uint8_t ANGLE_GRADIENT = 3;

  void cycle() {
    if (_paletteCycleTimer.complete()) {
      _setNextColorPalette();
      _paletteCycleTimer.reset();
    }

    EVERY_N_MILLISECONDS(10) {
      nblendPaletteTowardPalette(_currentPalette, _targetPalette, 12);
    }
  }

  void setColorMode(uint8_t colorMode) { _activeColorMode = colorMode; }

  void setSecondsPerPalette(int secondsPerPalette) {
    _paletteCycleTimer.totalCycleTime = secondsPerPalette * 1000;
  }

  int getSecondsPerPalette() {
    return _paletteCycleTimer.totalCycleTime / 1000;
  }

  uint8_t getNumPalettes() {
    return sizeof(activePalettes) / sizeof(activePalettes[0]);
  }

  void setPalette(uint8_t whichPalette) {
    _currentPalette = *(activePalettes[whichPalette]);
    _targetPalette = *(activePalettes[whichPalette]);
    _paletteCycleTimer.reset();
  }

  CRGB colorFromPalette(uint8_t paletteIndex) {
    return ColorFromPalette(_currentPalette, paletteIndex);
  }

  CRGB getColor(int i, bool reversePalette = false) {
    uint8_t paletteIndex = 0;
    switch (_activeColorMode) {
    case INDEX_GRADIENT: {
      paletteIndex = map(i, 0, NUM_LEDS - 1, 0, MAX_PALETTE_INDEX);
      break;
    }
    case DEPTH_GRADIENT: {
      paletteIndex = map(ledDepth(i), 0, MAX_DEPTH, 0, MAX_PALETTE_INDEX);
      break;
    }
    case ANGLE_GRADIENT: {
      paletteIndex = map(ledAngle(i), 0, 360, 0, MAX_PALETTE_INDEX);
      break;
    }
    default:
      break;
    }
    if (reversePalette) {
      paletteIndex = MAX_PALETTE_INDEX - paletteIndex;
    }
    return ColorFromPalette(_currentPalette, paletteIndex);
  }
};
