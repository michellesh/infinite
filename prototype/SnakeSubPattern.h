struct Snake {
  Node n1;
  Node n2;
  Node n3;
  Node n4;
};

Node n1 = createNode(1, 1);
Node n2 = createNode(1, 3);
Node n3 = createNode(2, 3);
Node n4 = createNode(2, 5);
Snake snake = {n1, n2, n3, n4};

class SnakeSubPattern : public SubPattern {
private:
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern
                                  //
  Node *_filterNodes(Node *options, int length, int &filteredLength,
                     Node excludeNode) {
    // Create a temporary array to hold the filtered nodes
    Node *temp = new Node[length];
    int count = 0;

    // Apply the filter condition
    for (int i = 0; i < length; i++) {
      bool isValidStraight =
          options[i].straightNum >= 0 && options[i].straightNum < NUM_STRAIGHTS;
      bool isValidRing =
          options[i].ringNum > 0 && options[i].ringNum < NUM_RINGS;
      bool isExcludeNode = options[i].ringNum == excludeNode.ringNum &&
                           options[i].straightNum == excludeNode.straightNum;
      if (isValidStraight && isValidRing && !isExcludeNode) {
        temp[count] = options[i];
        count++;
      }
    }

    // Create an array of the correct size to hold the filtered nodes
    Node *filteredNodes = new Node[count];
    for (int i = 0; i < count; i++) {
      filteredNodes[i] = temp[i];
    }

    // Set the filtered length
    filteredLength = count;

    // Free the temporary array
    delete[] temp;

    return filteredNodes;
  }

  Node _newNode(Node fromNode, Node toNode) {
    Node options[] = {createNode(toNode.straightNum + 1, toNode.ringNum),
                      createNode(toNode.straightNum - 1, toNode.ringNum),
                      createNode(toNode.straightNum, toNode.ringNum + 2),
                      createNode(toNode.straightNum, toNode.ringNum - 2)};

    int filteredLength = 4;
    Node *filteredNodes = _filterNodes(options, 4, filteredLength, fromNode);

    int choice = random(filteredLength);
    return filteredNodes[choice];
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
  void _showSnakes() {
    int percent = mapBeat(0, 100);
    static int prevPercent = percent;
    if (percent < prevPercent) {
      Node newNode = _newNode(snake.n3, snake.n4);
      snake.n1 = snake.n2;
      snake.n2 = snake.n3;
      snake.n3 = snake.n4;
      snake.n4 = newNode;
    }
    prevPercent = percent;

    // n1 to n2
    _drawSnakeFromNodeToNode(snake.n1, snake.n2, -1 * percent);
    _drawSnakeFromNodeToNode(snake.n2, snake.n3, 100);
    _drawSnakeFromNodeToNode(snake.n3, snake.n4, percent);
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
