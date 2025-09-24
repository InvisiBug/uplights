void startScreen() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    for (;;) {
      Serial << "Can't talk to screen" << endl;
      delay(1000);
    }
  }
  display.setRotation(0); // 2

  middleText(F("Uplight"));

  delay(2000);
}

void text(String text) {
  display.clearDisplay();

  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(text);
  display.display();
}

void middleText(String text) {
  display.clearDisplay();

  display.setTextSize(2);                        // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);           // Draw white text
  display.setCursor(middle(text.length()), 10);  // Start at top-left corner
  display.println(text);
  display.display();
}

void debugText(String text) {
  display.clearDisplay();

  display.setTextSize(2);                        // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);           // Draw white text
  display.setCursor(middle(text.length()), 10);  // Start at top-left corner
  display.println(text);
  display.display();
}

int middle(int textWidth) {
  //? Can show a total of 10 characters
  int screenWidth = display.width();
  int charWidth = 12;

  return (screenWidth / 2) - ((charWidth * textWidth) / 2);
}

void border() {
  display.clearDisplay();

  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  display.display();
}