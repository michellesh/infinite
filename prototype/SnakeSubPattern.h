#define NUM_SNAKES 10

struct Snake {
  Node n1;
  Node n2;
  Node n3;
  Node n4;
};

Snake snakes[NUM_SNAKES];

class SnakeSubPattern : public SubPattern {
private:
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern

  bool _isValidNode(Node n, Node excludeNode) {
    bool isValidStraight = n.straightNum >= 0 && n.straightNum < NUM_STRAIGHTS;
    bool isValidRing = excludeNode.ringNum % 2 == 1
                           ? (n.ringNum > 0 && n.ringNum < NUM_RINGS)
                           : (n.ringNum >= 0 && n.ringNum < NUM_RINGS - 1);
    bool isExcludeNode = n.ringNum == excludeNode.ringNum &&
                         n.straightNum == excludeNode.straightNum;
    return isValidStraight && isValidRing && !isExcludeNode;
  }

  Node _getRandomAdjacentNode(Node fromNode, Node toNode) {
    Node options[] = {createNode(toNode.straightNum + 1, toNode.ringNum),
                      createNode(toNode.straightNum - 1, toNode.ringNum),
                      createNode(toNode.straightNum, toNode.ringNum + 2),
                      createNode(toNode.straightNum, toNode.ringNum - 2)};

    bool validNode[] = {0, 0, 0, 0};
    int numValid = 0;
    for (int i = 0; i < 4; i++) {
      if (_isValidNode(options[i], fromNode)) {
        validNode[i] = true;
        numValid++;
      }
    }

    int choice = random(numValid);
    int index = 0;
    for (int i = 0; i < 4; i++) {
      if (validNode[i]) {
        if (index == choice) {
          index = i;
          break;
        } else {
          index++;
        }
      }
    }
    return options[index];
  }

  void _drawSnakeFromNodeToNode(Node n1, Node n2, int percent = 100) {
    int startPosition, endPosition;
    Path path;
    if (n1.straightNum == n2.straightNum) {
      path = straights[n1.straightNum];
      startPosition = n1.straightPosition;
      endPosition = n2.straightPosition;
    } else if (n1.ringNum == n2.ringNum) {
      path = rings[n1.ringNum];
      startPosition = n1.ringPosition;
      endPosition = n2.ringPosition;
    } else {
      Serial.println("Nodes not adjacent");
      return;
    }
    if (percent < 0) { // tail
      startPosition = map(percent, 0, -100, startPosition, endPosition);
    } else if (percent < 100) { // head
      endPosition = map(percent, 0, 100, startPosition, endPosition);
    }
    for (int i = min(startPosition, endPosition);
         i < max(startPosition, endPosition); i++) {
      path.leds[i] = palette.getColor(path.offset + i);
    }
  }

  //  SNAKES
  void _showSnake(Snake &snake, int percent, bool makeNewNode) {
    if (makeNewNode) {
      Node newNode = _getRandomAdjacentNode(snake.n3, snake.n4);
      snake.n1 = snake.n2;
      snake.n2 = snake.n3;
      snake.n3 = snake.n4;
      snake.n4 = newNode;
    }

    _drawSnakeFromNodeToNode(snake.n1, snake.n2, -1 * percent);
    _drawSnakeFromNodeToNode(snake.n2, snake.n3, 100);
    _drawSnakeFromNodeToNode(snake.n3, snake.n4, percent);
  }

  void _showSnakes() {
    int percent = mapBeat(0, 100);
    static int prevPercent = percent;
    bool makeNewNode = false;
    if (percent < prevPercent) {
      makeNewNode = true;
    }
    prevPercent = percent;

    for (int i = 0; i < NUM_SNAKES; i++) {
      _showSnake(snakes[i], percent, makeNewNode);
    }
  }

public:
  static const uint8_t SNAKES = PATTERN_SNAKES;

  SnakeSubPattern(uint8_t activeSubPattern = 0) {
    _activeSubPattern = activeSubPattern;
  }

  void setActivePattern(uint8_t activeSubPattern) {
    _activeSubPattern = activeSubPattern;
  }

  virtual uint8_t getPercentBrightness() { return _percentBrightness; }

  virtual void setPercentBrightness(uint8_t percentBrightness) {
    _percentBrightness = percentBrightness;
  }

  void setup() {
    switch (_activeSubPattern) {
    case SNAKES: {
      for (int i = 0; i < NUM_SNAKES; i++) {
        Node n4 = createNode(random(NUM_STRAIGHTS), random(NUM_RINGS));
        Node n3 = _getRandomAdjacentNode(n4, n4);
        Node n2 = _getRandomAdjacentNode(n4, n3);
        Node n1 = _getRandomAdjacentNode(n3, n2);
        snakes[i] = {n1, n2, n3, n4};
      }
      break;
    }
    default:
      break;
    }
  }

  virtual void show() {
    switch (_activeSubPattern) {
    case SNAKES:
      _showSnakes();
      break;
    default:
      break;
    }
  }
};
