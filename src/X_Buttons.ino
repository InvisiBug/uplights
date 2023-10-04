void startButtons() {
  upButton.attachClick(upButtonClicked);
  upButton.setDebounceTicks(50);
  upButton.attachLongPressStart(upButtonHeld);
  upButton.setPressTicks(250);

  downButton.attachClick(downButtonClicked);
  downButton.setDebounceTicks(50);
  downButton.attachLongPressStart(downButtonHeld);
  downButton.setPressTicks(250);

  leftButton.attachClick(leftButtonClicked);
  leftButton.setDebounceTicks(50);
  leftButton.attachLongPressStart(leftButtonHeld);
  leftButton.setPressTicks(250);

  rightButton.attachClick(rightButtonClicked);
  rightButton.setDebounceTicks(50);
  rightButton.attachLongPressStart(rightButtonHeld);
  rightButton.setPressTicks(250);

  acceptButton.attachClick(acceptButtonClicked);
  acceptButton.setDebounceTicks(50);
  acceptButton.attachLongPressStart(acceptButtonHeld);
  acceptButton.setPressTicks(250);
}

////////////////////////////////////////////////////////////////////////
//
//  ######                                        #
//  #     # #    # ##### #####  ####  #    #     ##
//  #     # #    #   #     #   #    # ##   #    # #
//  ######  #    #   #     #   #    # # #  #      #
//  #     # #    #   #     #   #    # #  # #      #
//  #     # #    #   #     #   #    # #   ##      #
//  ######   ####    #     #    ####  #    #    #####
//
///////////////////////////////////////////////////////////////////////
void upButtonClicked() {
  Serial << "Up Button Clicked" << endl;
  if (menu < addr) {
    menu++;
  } else {
    menu = 1;
  }
}

void upButtonHeld() {
  Serial << "Up Button Clicked" << endl;
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                       #####
//  #     # #    # ##### #####  ####  #    #    #     #
//  #     # #    #   #     #   #    # ##   #          #
//  ######  #    #   #     #   #    # # #  #     #####
//  #     # #    #   #     #   #    # #  # #    #
//  #     # #    #   #     #   #    # #   ##    #
//  ######   ####    #     #    ####  #    #    #######
//
///////////////////////////////////////////////////////////////////////
void downButtonClicked() {
  Serial << "Down Button Clicked" << endl;
  if (menu > start) {
    menu--;
  } else {
    menu = addr;
  }
}

void downButtonHeld() {
  Serial << "Down Button Held" << endl;
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                       #####
//  #     # #    # ##### #####  ####  #    #    #     #
//  #     # #    #   #     #   #    # ##   #          #
//  ######  #    #   #     #   #    # # #  #     #####
//  #     # #    #   #     #   #    # #  # #          #
//  #     # #    #   #     #   #    # #   ##    #     #
//  ######   ####    #     #    ####  #    #     #####
//
///////////////////////////////////////////////////////////////////////
void leftButtonClicked() {
  Serial << "Left Button Clicked" << endl;
  if (menu == addr) {
    if (address > 0) {
      address--;
    } else {
      address = 100;
    }
    EEPROM.write(0, address);
    EEPROM.commit();
  }
}

void leftButtonHeld() {
  Serial << "Left Button Held" << endl;
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                      #
//  #     # #    # ##### #####  ####  #    #    #    #
//  #     # #    #   #     #   #    # ##   #    #    #
//  ######  #    #   #     #   #    # # #  #    #    #
//  #     # #    #   #     #   #    # #  # #    #######
//  #     # #    #   #     #   #    # #   ##         #
//  ######   ####    #     #    ####  #    #         #
//
///////////////////////////////////////////////////////////////////////
void rightButtonClicked() {
  Serial << "Right button clicked" << endl;
  if (menu == addr) {
    if (address < 100) {
      address++;
    } else {
      address = 0;
    }
    EEPROM.write(0, address);
    EEPROM.commit();
  }
}

void rightButtonHeld() {
  Serial << "Right button held" << endl;
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                      #######
//  #     # #    # ##### #####  ####  #    #    #
//  #     # #    #   #     #   #    # ##   #    #
//  ######  #    #   #     #   #    # # #  #    ######
//  #     # #    #   #     #   #    # #  # #          #
//  #     # #    #   #     #   #    # #   ##    #     #
//  ######   ####    #     #    ####  #    #     #####
//
///////////////////////////////////////////////////////////////////////
void acceptButtonClicked() {
  Serial << "Accept button clicked" << endl;
}

void acceptButtonHeld() {
  Serial << "Accept button held" << endl;
}
