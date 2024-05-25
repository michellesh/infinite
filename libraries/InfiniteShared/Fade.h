struct Fade {
  unsigned long millisStart;
  unsigned long millisEnd;
  int volumeStart;
  int volumeEnd;

  int getVolume() {
    return map(millis(), millisStart, millisEnd, volumeStart, volumeEnd);
  }
};
