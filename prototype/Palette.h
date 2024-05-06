#define MAX_PALETTE_INDEX 245

class Palette {
private:
  CRGBPalette16 _currentPalette = *(activePalettes[0]);
  CRGBPalette16 _targetPalette = *(activePalettes[0]);
  uint8_t _activeColorMode = DEPTH_GRADIENT;
  uint8_t _secondsPerPalette = 10;
  Timer _paletteCycleTimer = {_secondsPerPalette * 1000};

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

  CRGB getColor(int i, bool reversePalette = false) {
    uint8_t paletteIndex = 0;
    switch (_activeColorMode) {
    case INDEX_GRADIENT: {
      paletteIndex = map(i, 0, NUM_LEDS - 1, 0, MAX_PALETTE_INDEX);
      break;
    }
    case DEPTH_GRADIENT: {
      paletteIndex = map(ledDepth[i], 0, MAX_DEPTH, 0, MAX_PALETTE_INDEX);
      break;
    }
    case ANGLE_GRADIENT: {
      paletteIndex = map(ledAngle[i], 0, 360, 0, MAX_PALETTE_INDEX);
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
