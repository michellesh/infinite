class Spiral : public Pattern {
private:
  uint8_t _id = 0;
  int16_t _angle = 0; // the current angle
  uint8_t _colorPaletteIndex = 0;
  float _speedMultiplier = 1;
  float _densityMultiplier = 1;

  int _getSpeed() { return (float)speed * _speedMultiplier; }

  int _getWidth() { return map(width, 1, 10, WIDTH.MIN, WIDTH.MAX); }

  int _getOffset() {
    float _density = density <= 5 ? map(density, 1, 5, -OFFSET.MAX, -OFFSET.MIN)
                                  : map(density, 5, 10, OFFSET.MIN, OFFSET.MAX);
    return _density * _densityMultiplier;
  }

  // Gets the brightness of the LED if the LED's angle is close to the current
  // angle
  uint8_t _getBrightness(int index) {

    int16_t angleOffset = ledDepth[index] * _getOffset();
    int16_t targetAngle = (_angle + angleOffset + 360) % 360;
    int16_t pixelAngle = ledAngle[index];
    int16_t _width = _getWidth();

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
      50, 300, 180}; // How many degrees along the circumference at
                     // the current angle to light up
  static constexpr Range SPEED = {1, 10, 3}; // How many degrees to add to the
                                             // current angle each time
  static constexpr Range OFFSET = {
      1, 4, 2}; // How many degrees to curl the spiral per unit of depth

  void setAngle(int16_t angle) { _angle = angle; }

  void setSpeedMultiplier(float speedMultiplier) {
    _speedMultiplier = speedMultiplier;
  }

  void setDensityMultiplier(float densityMultiplier) {
    _densityMultiplier = densityMultiplier;
  }

  void setColorPaletteIndex(int16_t colorPaletteIndex) {
    _colorPaletteIndex = colorPaletteIndex;
  }

  void reverse() { _speedMultiplier = _speedMultiplier * -1; }

  void show() {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t brightness = _getBrightness(i);
      if (brightness > 0) {
        CRGB color = palette.colorFromPalette(_colorPaletteIndex);
        leds[i] = color.nscale8(brightness);
      }
    }

    // Increment the angle. After 360 degrees, start over at 0 degrees
    _angle = (_angle + _getSpeed() + 360) % 360;
  }
};
