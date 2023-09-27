////////////////////////////////////////////////////////////////////////
//
// ###                                 #####
//  #  #    #   ##    ####  ######    #     # ###### #    # ###### #####    ##   #####  ####  #####
//  #  ##  ##  #  #  #    # #         #       #      ##   # #      #    #  #  #    #   #    # #    #
//  #  # ## # #    # #      #####     #  #### #####  # #  # #####  #    # #    #   #   #    # #    #
//  #  #    # ###### #  ### #         #     # #      #  # # #      #####  ######   #   #    # #####
//  #  #    # #    # #    # #         #     # #      #   ## #      #   #  #    #   #   #    # #   #
// ### #    # #    #  ####  ######     #####  ###### #    # ###### #    # #    #   #    ####  #    #
//
////////////////////////////////////////////////////////////////////////
void drawImage(unsigned long time, const char picture[][totalLEDs][3], int pictureWidth) {
  int currentTime = millis();  // Get the current time (Unix Time)
  int column;                  // This might need to be declared outside this function

  while ((millis() < currentTime + (time * 1000)) && showPicture) {
    if (column < pictureWidth) {
      for (int row = totalLEDs; row >= 0; row--) {
        currentLED[row].setRGB(
            picture[column][row][redVal],    // Red   (0)
            picture[column][row][greenVal],  // Green (1)
            picture[column][row][blueVal]);  // Blue  (2)
      }
      FastLED.show();
      column++;
      delayMicroseconds(40);  //* How wide the image is (800 used to be good but now 40 seems better, figure out why)
    }

    else {
      column = 0;
    }

    delayMicroseconds(1000);  //* Gap between images (1000 seems good)
    button.tick();            // This is here to prevent the strip being updated after a mode change
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  #####
// #     # #   #  ####  ##### ###### #    #
// #        # #  #        #   #      ##  ##
//  #####    #    ####    #   #####  # ## #
//       #   #        #   #   #      #    #
// #     #   #   #    #   #   #      #    #
//  #####    #    ####    #   ###### #    #
//
//////////////////////////////////////////////////////////////////////////////
void resetAll() {
  rainbow.reset();
  crissCross.reset();
  colourCycle.reset();
  colourFade.reset();

  showPicture = false;
  off(10);
}

void off(int wait) {
  setAll(0x000000);
  delay(wait);
}

void flash(int wait) {
  int totalFlashes = 2;
  for (int i = 0; i < totalFlashes; i++) {
    setAll(themeColour);
    delay(wait);
    off(wait);
  }
}

void setAll(int themeColour) {
  for (int i = 0; i < totalLEDs; i++) {
    currentLED[i] = themeColour;
  }
  FastLED.show();
}

void printStatus() {
  Serial << " Click Active : " << clickActive;
  Serial << " Long Press Active : " << longPressActive;
  Serial << " Double Click Active :" << doubleClickActive << endl;
}