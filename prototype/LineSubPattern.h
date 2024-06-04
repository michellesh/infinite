#define MAX_LINES                                                              \
  NUM_STRAIGHTS *(NUM_RINGS - 1) + NUM_RINGS *(NUM_STRAIGHTS - 1)
#define NUM_FLICKERS 4

#define MAX_IDLE_TIME 2000
#define NUM_NODES 30
#define NODE_LENGTH NUM_LEDS_PER_STRAIGHT / (NUM_RINGS - 1)

// Spinning hexagon patterns speed multiplier range
// min and max are range for variable speeds
RangeF spinningSpeedMultiplier = {0.25, 0.5, 0.5}; // min, max, default

int order[MAX_LINES];
int bouncePeak[NUM_RINGS];
float bouncePosition[NUM_RINGS];
int cometPosition[NUM_STRAIGHTS * 2];
bool cometActive[NUM_STRAIGHTS * 2];
int nextComet = 0;

struct Flicker {
  int id = 0;
  bool state = false;
  bool idle = false;
  Timer idTimer = {1000};
  Timer stateTimer = {10};
};

Flicker flickers[NUM_FLICKERS];

Node waveformNodes[NUM_NODES];

class LineSubPattern : public SubPattern {
private:
  Line _lines[MAX_LINES];
  uint8_t _numLines = MAX_LINES;
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  // ROTATING_PONG
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
      if (_lines[i].isFullyOutOfBounds()) {
        _lines[i].toggleReverse();
        _lines[i].ignoreNewPosition();
      } else {
        _lines[i].commitNewPosition();
      }
    }
  }

  // LASERS_ALL_AT_ONCE
  // RAINFALL_CYCLE_ON_BEAT
  // RAINFALL_COMET_TRAILS
  // BASKET_WEAVING
  void _showLines() {
    for (int i = 0; i < _numLines; i++) {
      _lines[i].show();
    }
  }

  // POWER_UP_AND_FIRE
  void _showPowerUpAndFire(bool broke = false) {
    unsigned long percent = mapBeat(0, 100);
    static unsigned long prevPercent = percent;
    static uint8_t beatNum = 0;
    int numBeats = 4;
    if (percent < prevPercent) {
      beatNum = (beatNum + 1) % numBeats;
    }
    prevPercent = percent;
    percent += 100 * beatNum;
    int amountPowerUp = 100 * (numBeats - 1); // percent time gaining power
    int amountTotalPower =
        amountPowerUp + 20; // total power time including drop
    if (percent < amountTotalPower) {
      int minPower = reverse ? 0 : MAX_DEPTH;
      int maxPower = reverse ? MAX_DEPTH * 3 / 4 : MAX_DEPTH / 4;
      int powerUpAmount =
          percent < amountPowerUp
              ? map(percent, 0, amountPowerUp, minPower, maxPower)
              : map(percent, amountPowerUp, amountTotalPower, maxPower,
                    minPower);
      for (int i = 0; i < NUM_LEDS; i++) {
        if ((!reverse && ledDepth(i) > powerUpAmount) ||
            (reverse && ledDepth(i) < powerUpAmount)) {
          int mapTo = broke ? MAX_DEPTH - minPower : minPower;
          uint8_t brightness = map(ledDepth(i), powerUpAmount, mapTo, 0, 255);
          leds[i] = palette.getColor(i).nscale8(brightness);
        }
      }
    } else {
      // lasers all at once
      for (int i = 0; i < _numLines; i++) {
        _lines[i].show();
      }
    }
  }

  // LASERS
  void _showOneByOne() {
    static int active = 0;
    float nextPosition = _lines[order[active]].getPosition();
    if (_lines[order[active]].isFullyOutOfBounds(nextPosition)) {
      active = (active + 1) % _numLines;
      _lines[order[active]].resetPrevPosition();
    }
    _lines[order[active]].show(nextPosition);
    _lines[order[active]].commitNewPosition();
  }

  // COMET_TRAILS
  void _showCometTrails() {
    static float inc = mapf(speed, 1, 10, 1, 5);
    static int prevSpeed = speed;
    if (prevSpeed != speed) {
      inc = mapf(speed, 1, 10, 1, 5);
    }
    prevSpeed = speed;
    if (beat) {
      // start a comet
      if (!cometActive[nextComet]) {
        cometPosition[nextComet] = _lines[nextComet].isReversed()
                                       ? 0
                                       : _lines[nextComet].getEndPosition(true);
        cometActive[nextComet] = true;
        nextComet = (nextComet + 1) % (NUM_STRAIGHTS * 2);
      }
    }
    for (int i = 0; i < NUM_STRAIGHTS * 2; i++) {
      if (cometActive[i]) {
        cometPosition[i] += _lines[i].isReversed() ? inc : -1 * inc;
        if ((_lines[i].isReversed() &&
             cometPosition[i] > _lines[i].getEndPosition(true)) ||
            (!_lines[i].isReversed() && cometPosition[i] < 0)) {
          cometActive[i] = false;
        }
        _lines[i].show(cometPosition[i]);
      }
    }
  }

  // LASERS_DOUBLES
  void _showTwoByTwo() {
    static int active = 0;
    static bool again = true;
    float nextPosition = _lines[order[active]].getPosition();
    if (_lines[order[active]].isFullyOutOfBounds(nextPosition)) {
      if (!again) {
        active = (active + 1) % _numLines;
        again = true;
      } else {
        again = false;
      }
      _lines[order[active]].resetPrevPosition();
    }
    _lines[order[active]].show(nextPosition);
    _lines[order[active]].commitNewPosition();
  }

  // RAINFALL_FALL_ON_BEAT
  void _showRainfall() {
    for (int i = 0; i < _numLines / 2; i++) {
      if (i == 0 && _lines[order[i]].isFullyOutOfBounds()) {
        shuffleIndexes(order, _numLines);
      }
      _lines[order[i]].commitNewPosition();
      _lines[order[i]].show();
    }
  }

  // ROTATING_HEXAGONS
  // COUNTER_ROTATING_HEXAGONS
  void _showRotatingHexagonsOffset() {
    static int _prevDensity = density;
    if (density != _prevDensity) {
      int degreeStep = map(density, 1, 10, 0, 50); // range 0-50 degree offset
      for (int i = 0; i < _numLines; i++) {
        _lines[i].setOffset(i * (degreeStep * NUM_LEDS_PER_RING / 360));
      }
      _prevDensity = density;
    }
    for (int i = 0; i < _numLines; i++) {
      _lines[i].showRepeat();
    }
  }

  // VARIABLE_SPEED_ROTATION
  // VARIABLE_SPEED_ROTATION_END
  void _showRotatingHexagons() {
    for (int i = 0; i < _numLines; i++) {
      _lines[i].showRepeat();
    }
  }

  // RANDOM_FLASHING_SEGMENTS
  void _showRandomFlashingSegments() {
    for (int i = 0; i < _numLines; i++) {
      _lines[i].showPathFixed(_numLines, false, true);
    }
  }

  // RANDOM_FLASHING_SEGMENTS_GLITCH
  void _showRandomFlashingSegmentsGlitch() {
    for (int i = 0; i < NUM_FLICKERS; i++) {
      if (flickers[i].idTimer.complete()) {
        flickers[i].idle = !flickers[i].idle;
        flickers[i].id = random(0, _numLines - 1);
        flickers[i].stateTimer.totalCycleTime = random(10, 100);
        flickers[i].idTimer.totalCycleTime = random(600, 1200);
        flickers[i].idTimer.reset();
      }
      if (flickers[i].stateTimer.complete()) {
        flickers[i].state = !flickers[i].state;
        flickers[i].stateTimer.reset();
      }
    }

    for (int i = 0; i < _numLines; i++) {
      bool isFlickering = false;
      bool flickerState = false;
      for (int j = 0; j < NUM_FLICKERS; j++) {
        if (!flickers[j].idle && flickers[j].id == i) {
          isFlickering = true;
          flickerState = flickers[j].state;
        }
      }
      _lines[i].showPathFixed(_numLines, isFlickering, flickerState);
    }
  }

  int PERCENT_BOUNCE_UP = 20; // percent of beat to bounce upward
  int PERCENT_MIN_BOUNCE =
      15; // min position of waveform as percent of path length

  void _updateBouncePosition(int bounceIndex, int minBouncePosition) {
    int percent = mapBeat(0, 100);
    if (percent < PERCENT_BOUNCE_UP) {
      bouncePosition[bounceIndex] =
          map(percent, 0, PERCENT_BOUNCE_UP, minBouncePosition,
              bouncePeak[bounceIndex]);
    } else {
      bouncePosition[bounceIndex] =
          map(percent, PERCENT_BOUNCE_UP, 100, bouncePeak[bounceIndex],
              minBouncePosition);
    }
  }

  void _updateBoucePeaks(float peakMax, bool force = false) {
    static int prevWidth = -1;
    if (prevWidth != width || force) {
      int peakRangeMin = map(width, 1, 10, peakMax * 0.4, peakMax * 0.7);
      int peakRangeMax = map(width, 1, 10, peakMax * 0.7, peakMax);
      for (int i = 0; i < NUM_RINGS; i++) {
        bouncePeak[i] = random(peakRangeMin, peakRangeMax);
      }
    }
    prevWidth = width;
  }

  // WAVEFORM_BOUNCING_SINGLE
  // WAVEFORM_BOUNCING_DOUBLE
  void _showWaveform(int bounceIndex = -1) {
    for (int i = 0; i < _numLines; i++) {
      int _bounceIndex = bounceIndex == -1 ? i % NUM_RINGS : bounceIndex;
      Path path = _lines[i].getPath();
      _updateBouncePosition(_bounceIndex,
                            path.length * PERCENT_MIN_BOUNCE / 100);
      if (!_lines[i].isReversed()) {
        for (int j = 0; j < bouncePosition[_bounceIndex]; j++) {
          uint8_t brightness =
              _lines[i].getFadeType() == Line::NO_FADE
                  ? 255
                  : map(j, 0, bouncePosition[_bounceIndex], 255, 0);
          path.leds[j] = palette.getColor(path.offset + j).nscale8(brightness);
        }
      } else {
        for (int j = path.length - 1;
             j > path.length - bouncePosition[_bounceIndex]; j--) {
          uint8_t brightness =
              _lines[i].getFadeType() == Line::NO_FADE
                  ? 255
                  : map(j, path.length - 1,
                        path.length - bouncePosition[_bounceIndex], 255, 0);
          path.leds[j] = palette.getColor(path.offset + j).nscale8(brightness);
        }
      }
    }
  }

  // WAVEFORM_BOUNCING_NODES
  bool _isNodePastEdge(int offset, Path basePath) {
    return offset + NODE_LENGTH > basePath.offset + basePath.length ||
           offset < basePath.offset;
  }

  void _setNodeLine(uint8_t lineIndex, Path basePath, int offset) {
    int _offset = basePath.offset + offset;
    Path path = {&leds[_offset], NODE_LENGTH, _offset};
    if (!_isNodePastEdge(_offset, basePath)) {
      _lines[lineIndex].setPath(path);
    }
  }

  void _showWaveformNodes() {
    Path path;
    int offset;
    for (int i = 0; i < NUM_NODES; i++) {
      Node node = waveformNodes[i];
      _setNodeLine(0, straights[node.straightNum],
                   node.straightPosition - NODE_LENGTH);
      _setNodeLine(1, straights[node.straightNum], node.straightPosition);
      _setNodeLine(2, rings[node.ringNum], node.ringPosition - NODE_LENGTH);
      _setNodeLine(3, rings[node.ringNum], node.ringPosition);
      _showWaveform(0);
    }
  }

public:
  static const uint8_t ROTATING_PONG = PATTERN_ROTATING_PONG;
  static const uint8_t LASERS = PATTERN_LASERS;
  static const uint8_t LASERS_DOUBLES = PATTERN_LASERS_DOUBLES;
  static const uint8_t LASERS_ALL_AT_ONCE = PATTERN_LASERS_ALL_AT_ONCE;
  static const uint8_t RAINFALL_CYCLE_ON_BEAT = PATTERN_RAINFALL_CYCLE_ON_BEAT;
  static const uint8_t RAINFALL_FALL_ON_BEAT = PATTERN_RAINFALL_FALL_ON_BEAT;
  static const uint8_t RAINFALL_COMET_TRAILS = PATTERN_RAINFALL_COMET_TRAILS;
  static const uint8_t BASKET_WEAVING = PATTERN_BASKET_WEAVING;
  static const uint8_t COMET_TRAILS = PATTERN_COMET_TRAILS;
  static const uint8_t ROTATING_HEXAGONS = PATTERN_ROTATING_HEXAGONS;
  static const uint8_t COUNTER_ROTATING_HEXAGONS =
      PATTERN_COUNTER_ROTATING_HEXAGONS;
  static const uint8_t VARIABLE_SPEED_ROTATION =
      PATTERN_VARIABLE_SPEED_ROTATION;
  static const uint8_t VARIABLE_SPEED_ROTATION_END =
      PATTERN_VARIABLE_SPEED_ROTATION_END;
  static const uint8_t RANDOM_FLASHING_SEGMENTS =
      PATTERN_RANDOM_FLASHING_SEGMENTS;
  static const uint8_t RANDOM_FLASHING_SEGMENTS_GLITCH =
      PATTERN_RANDOM_FLASHING_SEGMENTS_GLITCH;
  static const uint8_t WAVEFORM_BOUNCING_SINGLE =
      PATTERN_WAVEFORM_BOUNCING_SINGLE;
  static const uint8_t WAVEFORM_BOUNCING_DOUBLE =
      PATTERN_WAVEFORM_BOUNCING_DOUBLE;
  static const uint8_t WAVEFORM_BOUNCING_NODES =
      PATTERN_WAVEFORM_BOUNCING_NODES;
  static const uint8_t POWER_UP_AND_FIRE = PATTERN_POWER_UP_AND_FIRE;
  static const uint8_t POWER_UP_AND_FIRE_BROKEN =
      PATTERN_POWER_UP_AND_FIRE_BROKEN;

  LineSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setActivePattern(uint8_t activeSubPattern) {
    _activeSubPattern = activeSubPattern;
  }

  void setup() {
    switch (_activeSubPattern) {
    case ROTATING_PONG:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        int offsetStep = map(density, 1, 10, 0, 50);
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].setOffset(offsetStep * i);
        _lines[i].setLengthMultiplier(4);
        _lines[i].setReverse(false);
      }
      break;
    case LASERS:
    case LASERS_DOUBLES:
      _numLines = NUM_STRAIGHTS;
      shuffleIndexes(order, _numLines);
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].setLengthMultiplier(4);
        _lines[i].setReverse(false);
      }
      break;
    case LASERS_ALL_AT_ONCE:
    case POWER_UP_AND_FIRE:
    case POWER_UP_AND_FIRE_BROKEN:
      _numLines = NUM_STRAIGHTS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].setLengthMultiplier(4);
        _lines[i].setReverse(false);
      }
      break;
    case RAINFALL_CYCLE_ON_BEAT:
    case RAINFALL_COMET_TRAILS:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        if (_activeSubPattern == RAINFALL_COMET_TRAILS) {
          _lines[i].setFadeType(Line::FADE_COMET);
        }
        _lines[i].setSpeedMultiplier(0.5);
        _lines[i].setOffset(random(0, NUM_LEDS_PER_RING));
        if (i % 2 == 0) {
          int offset = rings[i].offset;
          Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(false);
        } else {
          int offset = rings[i].offset + NUM_LEDS_PER_RING / 2 - 1;
          Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(true);
        }
      }
      break;
    case RAINFALL_FALL_ON_BEAT:
      _numLines = NUM_RINGS;
      shuffleIndexes(order, _numLines);
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        if (i % 2 == 0) {
          int offset = rings[i].offset;
          Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(false);
        } else {
          int offset = rings[i].offset + NUM_LEDS_PER_RING / 2;
          Path path = {&leds[offset], NUM_LEDS_PER_RING / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(true);
        }
      }
      break;
    case BASKET_WEAVING:
      _numLines = NUM_STRAIGHTS + NUM_RINGS;
      shuffleIndexes(order, _numLines);
      for (uint8_t i = 0; i < NUM_STRAIGHTS; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(straights[i]);
        _lines[i].setReverse(true);
        _lines[i].setOffset(MAX_DEPTH - i * (MAX_DEPTH / NUM_STRAIGHTS));
      }
      for (uint8_t i = NUM_STRAIGHTS; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.25);
        _lines[i].setPath(rings[i - NUM_STRAIGHTS]);
        _lines[i].setReverse(true);
        _lines[i].setOffset(order[i] * (NUM_LEDS_PER_RING / NUM_RINGS));
      }
      break;
    case COMET_TRAILS:
      _numLines = NUM_STRAIGHTS * 2;
      shuffleIndexes(order, NUM_STRAIGHTS);
      nextComet = 0;
      for (uint8_t i = 0; i < _numLines; i++) {
        order[i + NUM_STRAIGHTS] = order[i]; // move shuffled values over
        cometPosition[i] = 0;
      }
      shuffleIndexes(order, NUM_STRAIGHTS); // shuffle again
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setSpeedMultiplier(0.1);
        _lines[i].setPath(straights[order[i]]);
        _lines[i].setLengthMultiplier(2);
        _lines[i].setFadeType(Line::FADE_COMET);
      }
      break;
    case ROTATING_HEXAGONS:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        int degreeStep = map(density, 1, 10, 0, 50); // range 0-50 degree offset
        _lines[i].setOffset(i * (degreeStep * NUM_LEDS_PER_RING / 360));
        _lines[i].setPath(rings[i]);
        _lines[i].setLengthMultiplier(0.5);
        _lines[i].setSpeedMultiplier(spinningSpeedMultiplier.DFLT);
        _lines[i].setReverse(false);
      }
      break;
    case COUNTER_ROTATING_HEXAGONS:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(rings[i]);
        _lines[i].setLengthMultiplier(0.5);
        _lines[i].setSpeedMultiplier(spinningSpeedMultiplier.DFLT);
        int degreeStep = map(density, 1, 10, 0, 50); // range 0-50 degree offset
        _lines[i].setOffset(i * (degreeStep * NUM_LEDS_PER_RING / 360));
        _lines[i].setReverse(i % 2 == 0);
      }
      break;
    case VARIABLE_SPEED_ROTATION:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(rings[i]);
        _lines[i].setLengthMultiplier(0.5);
        _lines[i].setReverse(false);
        _lines[i].setOffset(0);
        if (i < _numLines / 2) {
          _lines[i].setSpeedMultiplier(mapf(i, 0, _numLines / 2 - 1,
                                            spinningSpeedMultiplier.MIN,
                                            spinningSpeedMultiplier.MAX));
        } else {
          _lines[i].setSpeedMultiplier(mapf(i, _numLines / 2, _numLines - 1,
                                            spinningSpeedMultiplier.MAX,
                                            spinningSpeedMultiplier.MIN));
        }
      }
      break;
    case VARIABLE_SPEED_ROTATION_END:
      _numLines = NUM_RINGS;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setPath(rings[i]);
        _lines[i].setLengthMultiplier(0.5);
        _lines[i].setOffset(0);
        _lines[i].setSpeedMultiplier(mapf(i, 0, _numLines - 1,
                                          spinningSpeedMultiplier.MIN,
                                          spinningSpeedMultiplier.MAX));
        _lines[i].setReverse(false);
      }
      break;
    case RANDOM_FLASHING_SEGMENTS:
    case RANDOM_FLASHING_SEGMENTS_GLITCH:
      _numLines =
          NUM_STRAIGHTS * (NUM_RINGS - 1) + NUM_RINGS * (NUM_STRAIGHTS - 1);
      for (uint8_t i = 0; i < NUM_STRAIGHTS; i++) {
        int totalLength = 0;
        int segmentLength = (NUM_LEDS_PER_STRAIGHT / (NUM_RINGS - 1));
        for (uint8_t j = 0; j < NUM_RINGS - 1; j++) {
          int length = j == NUM_RINGS - 2 ? straights[i].length - totalLength
                                          : segmentLength;
          Path p = {&straights[i].leds[j * segmentLength], length,
                    straights[i].offset + j * segmentLength};
          int lineIndex = i * (NUM_RINGS - 1) + j;
          _lines[lineIndex] = Line(lineIndex);
          _lines[lineIndex].setPath(p);
          _lines[lineIndex].setSpeedMultiplier(1.5);
          totalLength += segmentLength;
        }
      }
      for (uint8_t i = 0; i < NUM_RINGS; i++) {
        int totalLength = 0;
        int segmentLength = NUM_LEDS_PER_RING / (NUM_STRAIGHTS - 1);
        for (uint8_t j = 0; j < NUM_STRAIGHTS - 1; j++) {
          int length = j == NUM_STRAIGHTS - 2 ? rings[i].length - totalLength
                                              : segmentLength;
          Path p = {&rings[i].leds[j * segmentLength], length,
                    rings[i].offset + j * segmentLength};
          int lineIndex =
              (NUM_STRAIGHTS * (NUM_RINGS - 1)) + i * (NUM_STRAIGHTS - 1) + j;
          _lines[lineIndex] = Line(lineIndex);
          _lines[lineIndex].setPath(p);
          _lines[lineIndex].setSpeedMultiplier(1.5);
          totalLength += segmentLength;
        }
      }
      break;
    case WAVEFORM_BOUNCING_SINGLE:
      _updateBoucePeaks(NUM_LEDS_PER_RING / 2, true);
      _numLines = NUM_RINGS * 2;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setFadeType(Line::NO_FADE);
        Path ring = rings[i % NUM_RINGS];
        if (i < _numLines / 2) {
          Path path = {&leds[ring.offset], ring.length / 2, ring.offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(false);
        } else {
          int offset = ring.offset + ring.length / 2;
          Path path = {&leds[offset], ring.length / 2, offset};
          _lines[i].setPath(path);
          _lines[i].setReverse(true);
        }
      }
      break;
    case WAVEFORM_BOUNCING_DOUBLE:
      _updateBoucePeaks(NUM_LEDS_PER_RING / 4, true);
      _numLines = NUM_RINGS * 4;
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setReverse(false);
        _lines[i].setFadeType(Line::NO_FADE);
        int offset;
        if (i < _numLines / 4) {
          offset = rings[i % NUM_RINGS].offset + NUM_LEDS_PER_RING / 5;
        } else if (i < _numLines / 2) {
          offset = rings[i % NUM_RINGS].offset + NUM_LEDS_PER_RING / 5 * 3 - 1;
          _lines[i].setReverse(true);
        } else if (i < _numLines / 4 * 3) {
          offset = rings[i % NUM_RINGS].offset;
          _lines[i].setReverse(true);
        } else {
          offset = rings[i % NUM_RINGS].offset + NUM_LEDS_PER_RING / 5 * 4 - 1;
        }
        Path path = {&leds[offset], NUM_LEDS_PER_RING / 5 + 1, offset};
        _lines[i].setPath(path);
      }
      break;
    case WAVEFORM_BOUNCING_NODES: {
      _numLines = 4; // 4 lines per node
      bouncePeak[0] = NODE_LENGTH;
      int n = 0;
      for (int i = 0; i < NUM_STRAIGHTS; i++) {
        for (int j = i % 2 == 0 ? 0 : 1; j < NUM_RINGS; j += 2) {
          waveformNodes[n] = createNode(i, j);
          n++;
        }
      }
      for (uint8_t i = 0; i < _numLines; i++) {
        _lines[i] = Line(i);
        _lines[i].setReverse(i % 2 == 0);
        _lines[i].setFadeType(Line::FADE_LIGHT);
      }
      break;
    }
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
    case LASERS:
      _showOneByOne();
      break;
    case COMET_TRAILS:
      //_showOneByOne();
      _showCometTrails();
      break;
    case LASERS_DOUBLES:
      _showTwoByTwo();
      break;
    case LASERS_ALL_AT_ONCE:
    case RAINFALL_CYCLE_ON_BEAT:
    case RAINFALL_COMET_TRAILS:
    case BASKET_WEAVING:
      _showLines();
      break;
    case RAINFALL_FALL_ON_BEAT:
      _showRainfall();
      break;
    case ROTATING_HEXAGONS:
    case COUNTER_ROTATING_HEXAGONS:
      _showRotatingHexagonsOffset();
      break;
    case VARIABLE_SPEED_ROTATION:
    case VARIABLE_SPEED_ROTATION_END:
      _showRotatingHexagons();
      break;
    case RANDOM_FLASHING_SEGMENTS:
      _showRandomFlashingSegments();
      break;
    case RANDOM_FLASHING_SEGMENTS_GLITCH:
      _showRandomFlashingSegmentsGlitch();
      break;
    case WAVEFORM_BOUNCING_SINGLE:
      _updateBoucePeaks(NUM_LEDS_PER_RING / 2);
      _showWaveform();
      break;
    case WAVEFORM_BOUNCING_DOUBLE:
      _updateBoucePeaks(NUM_LEDS_PER_RING / 4);
      _showWaveform();
      break;
    case WAVEFORM_BOUNCING_NODES:
      _showWaveformNodes();
      break;
    case POWER_UP_AND_FIRE:
      _showPowerUpAndFire();
      break;
    case POWER_UP_AND_FIRE_BROKEN:
      _showPowerUpAndFire(true);
      break;
    default:
      break;
    }
  }
};
