// #define MAX_LINES NUM_RINGS * 2
#define MAX_LINES                                                              \
  NUM_STRAIGHTS *(NUM_RINGS - 1) + NUM_RINGS *(NUM_STRAIGHTS - 1)
#define NUM_FLICKERS 4

#define MAX_IDLE_TIME 2000

// struct Flicker {
//   int id = 0;
//   bool state = false;
//   bool idle = false;
//   Timer idTimer = {1000};
//   Timer stateTimer = {10};
// };
//
// Flicker flickers[NUM_FLICKERS];

class LineSubPatternBPM : public SubPattern {
private:
  LineBPM _lines[MAX_LINES];
  uint8_t _numLines = MAX_LINES;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  void _showRotatingPong() {
    static int _prevDensity = 0;
    if (density != _prevDensity) {
      int offsetStep = map(density, 1, 10, 0, 50);
      for (int i = 0; i < _numLines; i++) {
        _lines[i].setOffset(offsetStep * i);
        _lines[i].resetPrevPosition();
      }
      _prevDensity = density;
    }
    for (int i = 0; i < _numLines; i++) {
      _lines[i].show();
      _lines[i].reverseOnWrap();
    }
  }

  void _showBasicReset() {
    for (int i = 0; i < _numLines; i++) {
      _lines[i].show();
    }
  }

  //void _showRandomReset() {
  //  for (int i = 0; i < _numLines; i++) {
  //    if (_lines[i].isFullyOutOfBounds()) {
  //      int pathLength = _lines[i].getPath().length;
  //      int maxOffset = map(density, 1, 10, pathLength * 7, 0);
  //      int offset = random(0, maxOffset);
  //      _lines[i].resetPosition(offset);
  //    }
  //    _lines[i].show();
  //  }
  //}

  //void _showRainfall() {
  //  for (int i = 0; i < _numLines; i++) {
  //    if (_lines[i].isFullyOutOfBounds()) {
  //      int r = random(0, 50);
  //      if (_lines[i].isReversed()) { // even numbered rings
  //        int max = NUM_LEDS_PER_RING / 2 + _lines[i].getLength();
  //        _lines[i].setPosition(max + r);
  //      } else {
  //        _lines[i].setPosition(-r);
  //      }
  //    }
  //    _lines[i].show();
  //  }
  //}

  //void _showRotatingHexagons() {
  //  static int _prevDensity = density;
  //  if (density != _prevDensity) {
  //    int degreeStep = map(density, 1, 10, 0, 50); // range 0-50 degree offset
  //    for (int i = 0; i < _numLines; i++) {
  //      _lines[i].setPosition(i * (degreeStep * NUM_LEDS_PER_RING / 360));
  //    }
  //    _prevDensity = density;
  //  }

  //  for (int i = 0; i < _numLines; i++) {
  //    float position = _lines[i].getPosition();
  //    Path path = _lines[i].getPath();
  //    bool isReversed = _lines[i].isReversed();
  //    float length = _lines[i].getLength();
  //    if (position >= (path.length) && !isReversed) {
  //      _lines[i].setPosition(position - path.length);
  //    } else if (position < 0 && isReversed) {
  //      _lines[i].setPosition(position + path.length - 1);
  //    }
  //    _lines[i].showRepeat();
  //  }
  //}

  // void _showRandomFlashingSegments() {
  //   for (int i = 0; i < NUM_FLICKERS; i++) {
  //     if (flickers[i].idTimer.complete()) {
  //       flickers[i].idle = !flickers[i].idle;
  //       flickers[i].id = random(0, _numLines - 1);
  //       flickers[i].stateTimer.totalCycleTime = random(10, 100);
  //       flickers[i].idTimer.totalCycleTime = random(600, 1200);
  //       flickers[i].idTimer.reset();
  //     }
  //     if (flickers[i].stateTimer.complete()) {
  //       flickers[i].state = !flickers[i].state;
  //       flickers[i].stateTimer.reset();
  //     }
  //   }

  //  for (int i = 0; i < _numLines; i++) {
  //    bool isFlickering = false;
  //    bool flickerState = false;
  //    for (int j = 0; j < NUM_FLICKERS; j++) {
  //      if (!flickers[j].idle && flickers[j].id == i) {
  //        isFlickering = true;
  //        flickerState = flickers[j].state;
  //      }
  //    }
  //    _lines[i].showPathFixed(_numLines, isFlickering, flickerState);
  //  }
  //}

public:
  static const uint8_t ROTATING_PONG = 0;
  static const uint8_t LASERS = 1;
  static const uint8_t LASERS_ALL_AT_ONCE = 2;
  static const uint8_t RAINFALL = 3;
  static const uint8_t BASKET_WEAVING = 4;
  static const uint8_t COMET_TRAILS = 5;
  static const uint8_t ROTATING_HEXAGONS = 6;
  static const uint8_t COUNTER_ROTATING_HEXAGONS = 7;
  static const uint8_t VARIABLE_SPEED_ROTATION = 8;
  static const uint8_t VARIABLE_SPEED_ROTATION_END = 9;
  static const uint8_t RANDOM_FLASHING_SEGMENTS = 10;

  LineSubPatternBPM(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case ROTATING_PONG:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = LineBPM(i);
        _lines[i].setPath(straights[i]);
      }
      break;
    //case LASERS:
    //  _numLines = NUM_STRAIGHTS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setPath(straights[i]);
    //    _lines[i].setReverse(true);
    //    _lines[i].setLengthMultiplier(2);
    //    _lines[i].setPosition(random(0, NUM_LEDS_PER_STRAIGHT));
    //  }
    //  break;
    case LASERS_ALL_AT_ONCE:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = LineBPM(i);
        _lines[i].setPath(straights[i]);
        //_lines[i].setReverse(true);
        _lines[i].setLengthMultiplier(2);
        //_lines[i].setPosition(0);
      }
      break;
    //case RAINFALL:
    //  _numLines = NUM_RINGS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setSpeedMultiplier(0.5);
    //    if (i % 2 == 0) {
    //      int offset = rings[i].offset;
    //      Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
    //      _lines[i].setPath(path);
    //      _lines[i].setPosition(NUM_LEDS_PER_RING / 2 + _lines[i].getLength());
    //      _lines[i].setReverse(true);
    //    } else {
    //      int offset = rings[i].offset + NUM_LEDS_PER_RING / 2;
    //      Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
    //      _lines[i].setPath(path);
    //      _lines[i].setPosition(0);
    //    }
    //  }
    //  break;
    //case BASKET_WEAVING:
    //  _numLines = NUM_STRAIGHTS + NUM_RINGS;
    //  for (uint8_t i = 0; i < NUM_STRAIGHTS; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setPath(straights[i]);
    //    _lines[i].setReverse(true);
    //    _lines[i].setPosition(MAX_DEPTH - i * (MAX_DEPTH / NUM_STRAIGHTS));
    //  }
    //  for (uint8_t i = NUM_STRAIGHTS; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setSpeedMultiplier(0.5);
    //    _lines[i].setPath(rings[i - NUM_STRAIGHTS]);
    //    _lines[i].setReverse(true);
    //    _lines[i].setPosition(360 - (i - NUM_STRAIGHTS) * (360 / NUM_RINGS));
    //    // TODO this shouldn't be 360
    //    // although that might be why it starts with a delay which is neat
    //  }
    //  break;
    //case COMET_TRAILS:
    //  _numLines = NUM_STRAIGHTS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setSpeedMultiplier(0.2);
    //    _lines[i].setPath(straights[i]);
    //    _lines[i].setLengthMultiplier(2);
    //    _lines[i].setFadeType(LineBPM::FADE_COMET);
    //    _lines[i].setReverse(true);
    //    _lines[i].setPosition(random(0, NUM_LEDS_PER_STRAIGHT));
    //  }
    //  break;
    //case ROTATING_HEXAGONS:
    //  _numLines = NUM_RINGS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setSpeedMultiplier(0.5);
    //    _lines[i].setPath(rings[i]);
    //    _lines[i].setLengthMultiplier(0.5);
    //    _lines[i].setPosition(i *
    //                          (10 * NUM_LEDS_PER_RING / 360)); // 10 "degrees"
    //  }
    //  break;
    //case COUNTER_ROTATING_HEXAGONS:
    //  _numLines = NUM_RINGS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setSpeedMultiplier(0.5);
    //    _lines[i].setPath(rings[i]);
    //    _lines[i].setLengthMultiplier(0.5);
    //    if (i % 2 == 0) {
    //      _lines[i].setReverse(true);
    //    }
    //    _lines[i].setPosition(i *
    //                          (10 * NUM_LEDS_PER_RING / 360)); // 10 "degrees"
    //  }
    //  break;
    //case VARIABLE_SPEED_ROTATION:
    //  _numLines = NUM_RINGS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setPath(rings[i]);
    //    _lines[i].setLengthMultiplier(0.5);
    //    if (i < _numLines / 2) {
    //      _lines[i].setSpeedMultiplier(mapf(i, 0, _numLines / 2 - 1, 0.2, 1));
    //    } else {
    //      _lines[i].setSpeedMultiplier(
    //          mapf(i, _numLines / 2, _numLines - 1, 1, 0.2));
    //    }
    //  }
    //  break;
    //case VARIABLE_SPEED_ROTATION_END:
    //  _numLines = NUM_RINGS;
    //  for (uint8_t i = 0; i < _numLines; i++) {
    //    _lines[i] = LineBPM(i);
    //    _lines[i].setPath(rings[i]);
    //    _lines[i].setLengthMultiplier(0.5);
    //    _lines[i].setSpeedMultiplier(mapf(i, 0, _numLines - 1, 0.2, 1));
    //  }
    //  break;
    //case RANDOM_FLASHING_SEGMENTS:
    //  _numLines =
    //      NUM_STRAIGHTS * (NUM_RINGS - 1) + NUM_RINGS * (NUM_STRAIGHTS - 1);
    //  for (uint8_t i = 0; i < NUM_STRAIGHTS; i++) {
    //    int totalLength = 0;
    //    int segmentLength = (NUM_LEDS_PER_STRAIGHT / (NUM_RINGS - 1));
    //    for (uint8_t j = 0; j < NUM_RINGS - 1; j++) {
    //      int length = j == NUM_RINGS - 2 ? straights[i].length - totalLength
    //                                      : segmentLength;
    //      Path p = {&straights[i].leds[j * segmentLength], length,
    //                straights[i].offset + j * segmentLength};
    //      int lineIndex = i * (NUM_RINGS - 1) + j;
    //      _lines[lineIndex] = LineBPM(lineIndex);
    //      _lines[lineIndex].setPath(p);
    //      _lines[lineIndex].setSpeedMultiplier(1.5);
    //      totalLength += segmentLength;
    //    }
    //  }
    //  for (uint8_t i = 0; i < NUM_RINGS; i++) {
    //    int totalLength = 0;
    //    int segmentLength = NUM_LEDS_PER_RING / (NUM_STRAIGHTS - 1);
    //    for (uint8_t j = 0; j < NUM_STRAIGHTS - 1; j++) {
    //      int length = j == NUM_STRAIGHTS - 2 ? rings[i].length - totalLength
    //                                          : segmentLength;
    //      Path p = {&rings[i].leds[j * segmentLength], length,
    //                rings[i].offset + j * segmentLength};
    //      int lineIndex =
    //          (NUM_STRAIGHTS * (NUM_RINGS - 1)) + i * (NUM_STRAIGHTS - 1) + j;
    //      _lines[lineIndex] = LineBPM(lineIndex);
    //      _lines[lineIndex].setPath(p);
    //      _lines[lineIndex].setSpeedMultiplier(1.5);
    //      totalLength += segmentLength;
    //    }
    //  }
    //  break;
    default:
      break;
    }
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
    for (uint8_t i = 0; i < _numLines; i++) {
      _lines[i].setPercentBrightness(percentBrightness);
    }
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case ROTATING_PONG:
      _showRotatingPong();
      break;
    //case LASERS:
    //  _showRandomReset();
    //  break;
    case LASERS_ALL_AT_ONCE:
      _showBasicReset();
      break;
    //case RAINFALL:
    //  _showRainfall();
    //  break;
    //case BASKET_WEAVING:
    //  _showRandomReset();
    //  break;
    //case COMET_TRAILS:
    //  _showRandomReset();
    //  break;
    //case ROTATING_HEXAGONS:
    //case COUNTER_ROTATING_HEXAGONS:
    //case VARIABLE_SPEED_ROTATION:
    //case VARIABLE_SPEED_ROTATION_END:
    //  _showRotatingHexagons();
    //  break;
    // case RANDOM_FLASHING_SEGMENTS:
    //   _showRandomFlashingSegments();
    //   break;
    default:
      break;
    }
  }
};
