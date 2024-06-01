struct Snode {
  int straightNum;
  int ringNum;
  int straightPosition;
  int ringPosition;
};

struct Snake {
  Snode n1;
  Snode n2;
  Snode n3;
  Snode n4;
};

Snode createNode(int straightNum, int ringNum) {
  return {straightNum, ringNum, straightPositionOnRing(ringNum),
          ringPositionOnStraight(straightNum)};
}

Snode n1 = createNode(1, 1);
Snode n2 = createNode(1, 3);
Snode n3 = createNode(2, 3);
Snode n4 = createNode(2, 5);
Snake snake = {n1, n2, n3, n4};

class SnakeSubPattern : public SubPattern {
private:
  uint8_t _activeSubPattern = 0;
  uint8_t _percentBrightness = 0; // percent brightness of the whole pattern
                                  //
  Snode *_filterNodes(Snode *options, int length, int &filteredLength,
                      Snode excludeNode) {
    // Create a temporary array to hold the filtered nodes
    Snode *temp = new Snode[length];
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
    Snode *filteredNodes = new Snode[count];
    for (int i = 0; i < count; i++) {
      filteredNodes[i] = temp[i];
    }

    // Set the filtered length
    filteredLength = count;

    // Free the temporary array
    delete[] temp;

    return filteredNodes;
  }

  Snode _newNode(Snode fromNode, Snode toNode) {
    Snode options[] = {createNode(toNode.straightNum + 1, toNode.ringNum),
                       createNode(toNode.straightNum - 1, toNode.ringNum),
                       createNode(toNode.straightNum, toNode.ringNum + 2),
                       createNode(toNode.straightNum, toNode.ringNum - 2)};

    int filteredLength = 4;
    Snode *filteredNodes = _filterNodes(options, 4, filteredLength, fromNode);

    int choice = random(filteredLength);
    return filteredNodes[choice];
  }

  void _drawSnakeFromNodeToNode(Snode n1, Snode n2, int percent = 100) {
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
      Snode newNode = _newNode(snake.n3, snake.n4);
      Serial.print("newNode: {");
      Serial.print(newNode.straightNum);
      Serial.print(", ");
      Serial.print(newNode.ringNum);
      Serial.println("}");

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
