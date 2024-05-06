class Spiral : public Pattern {
private:
  uint8_t _id = 0;
  int16_t _angle = 0; // the current angle
  int16_t _width = WIDTH.DFLT;
  int16_t _speed = SPEED.DFLT;
  int16_t _offset = OFFSET.DFLT;

  // Gets the brightness of the LED if the LED's angle is close to the current
  // angle
  uint8_t _getBrightness(int index) {

    int16_t angleOffset = ledHeight[index] * _offset;
    int16_t targetAngle = (_angle + angleOffset + 360) % 360;
    int16_t pixelAngle = ledAngle[index];

    // Calculate distance from the current angle. If angle is near end
    // (360-width < a < 360), also check LEDs near beginning. If angle is near
    // beginning (0 < a < width), also check LEDs near end
    int16_t dist = isBetween(targetAngle - pixelAngle, 0, _width)
                       ? targetAngle - pixelAngle
                   : targetAngle > 360 - _width &&
                           isBetween(targetAngle - 360 - pixelAngle, 0, _width)
                       ? targetAngle - 360 - pixelAngle
                   : targetAngle < _width &&
                           isBetween(targetAngle + 360 - pixelAngle, 0, _width)
                       ? targetAngle + 360 - pixelAngle
                       : 0;

    return addFadeShape(map(dist, 0, _width, 0, 255));
  };

public:
  Spiral(uint8_t id = 0) { _id = id; }

  static constexpr Range WIDTH = {
      10, 90, 90}; // How many degrees along the circumference at
                   // the current angle to light up
  static constexpr Range SPEED = {1, 10, 3}; // How many degrees to add to the
                                             // current angle each time
  static constexpr Range OFFSET = {
      1, 4, 2}; // How many degrees to curl the spiral per unit of height

  void setWidth(int16_t width) { _width = abs(width); }

  void setSpeed(int16_t speed) { _speed = speed; }

  void setAngle(int16_t angle) { _angle = angle; }

  void setOffset(int16_t offset) { _offset = offset; }

  void reverse() { _speed = _speed * -1; }

  void show() {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t brightness = _getBrightness(i);
      if (brightness > 0) {
        CRGB color = CRGB(100 * _id, 100, 100);
        leds[i] = color.nscale8(brightness);
      }
    }

    // Increment the angle. After 360 degrees, start over at 0 degrees
    _angle = (_angle + _speed + 360) % 360;
  }
};
