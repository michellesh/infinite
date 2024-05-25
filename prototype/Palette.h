#define MAX_PALETTE_INDEX 245

class Palette {
private:
  CRGBPalette16 _currentPalette = *(activePalettes[DEFAULT_PALETTE]);
  CRGBPalette16 _targetPalette = *(activePalettes[DEFAULT_PALETTE]);
  uint8_t _activeColorMode = DEFAULT_COLORMODE;
  uint8_t _activePalette = DEFAULT_PALETTE;

public:
  static const uint8_t SOLID = COLOR_MODE_SOLID;
  static const uint8_t INDEX_GRADIENT = COLOR_MODE_INDEX_GRADIENT;
  static const uint8_t DEPTH_GRADIENT = COLOR_MODE_DEPTH_GRADIENT;
  static const uint8_t ANGLE_GRADIENT = COLOR_MODE_ANGLE_GRADIENT;
  static const uint8_t CUSTOM_GRADIENT = COLOR_MODE_CUSTOM_GRADIENT;

  void cycle() {

    EVERY_N_MILLISECONDS(10) {
      nblendPaletteTowardPalette(_currentPalette, _targetPalette, 12);
    }
  }

  void setColorMode(uint8_t colorMode) { _activeColorMode = colorMode; }

  uint8_t getColorMode() { return _activeColorMode; }

  void setPalette(uint8_t whichPalette) {
    _activePalette = whichPalette;
    _targetPalette = *(activePalettes[whichPalette]);
  }

  uint8_t getPalette() { return _activePalette; }

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
    case CUSTOM_GRADIENT: {
      float depth = ledDepth(i);
      if (depth < MAX_DEPTH / 2) {
        float percent = mapf(depth, 0, MAX_DEPTH / 2, 0, 1);
        return getColorBetween(customColor1, customColor2, percent);
      } else {
        float percent = mapf(depth, MAX_DEPTH / 2, MAX_DEPTH, 0, 1);
        return getColorBetween(customColor2, customColor3, percent);
      }
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
