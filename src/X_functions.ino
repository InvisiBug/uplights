void manualMode() {
  int potMax = 4095;
  int potMin = 250;
  int redPercentage, greenPercentage, bluePercentage;

  //* Limit raw values
  int red = limit(analogRead(redPot), potMin, potMax);
  int green = limit(analogRead(greenPot), potMin, potMax);
  int blue = limit(analogRead(bluePot), potMin, potMax);

  //* Convert to percentages
  redPercentage = map(red, 0, potMax, 0, 100);
  greenPercentage = map(green, 0, potMax, 0, 100);
  bluePercentage = map(blue, 0, potMax, 0, 100);

  //* Convert to 0-255
  int finalRed = map(redPercentage, 0, 100, 255, 0);
  int finalGreen = map(greenPercentage, 0, 100, 255, 0);
  int finalBlue = map(bluePercentage, 0, 100, 255, 0);

  //* Set LEDs
  for (int i = 0; i < totalLEDs; i++) {
    currentLED[i].setRGB(finalRed, finalGreen, finalBlue);
  }

  FastLED.show();
}

int limit(int val, int min, int max) {
  int adjustedVal;

  if (val < min) {
    adjustedVal = 0;
  } else if (val > max) {
    adjustedVal = max;
  } else {
    adjustedVal = val;
  }

  return adjustedVal;
}