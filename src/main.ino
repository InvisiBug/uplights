// Dual core ESP32 tutorial =>https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
////////////////////////////////////////////////////////////////////////
//  ###
//   #  #    #  ####  #      #    # #####  ######  ####
//   #  ##   # #    # #      #    # #    # #      #
//   #  # #  # #      #      #    # #    # #####   ####
//   #  #  # # #      #      #    # #    # #           #
//   #  #   ## #    # #      #    # #    # #      #    #
//  ### #    #  ####  ######  ####  #####  ######  ####
//
////////////////////////////////////////////////////////////////////////
// Frameworks
#include <Adafruit_SSD1306.h>
#include <OneButton.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Wire.h>

#include "FastLED.h"
#include "Streaming.h"
#include "WiFi.h"

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # ###### ###### # #    # # ##### #  ####  #    #  ####
//  #     # #      #      # ##   # #   #   # #    # ##   # #
//  #     # #####  #####  # # #  # #   #   # #    # # #  #  ####
//  #     # #      #      # #  # # #   #   # #    # #  # #      #
//  #     # #      #      # #   ## #   #   # #    # #   ## #    #
//  ######  ###### #      # #    # #   #   #  ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
#define totalLEDs 8
// #define totalLEDs 60 // LEDs in the cloud

#define LED_BUILTIN 2  // ESP32, nothing required for ESP8266
#define connectionLED LED_BUILTIN

#define dataPin 23  // ESP32

#define upButtonPin 15
#define downButtonPin 4
#define leftButtonPin 5
#define rightButtonPin 18
#define acceptButtonPin 19

#define redPot 32
#define greenPot 39
#define bluePot 36

#define OFF LOW
#define ON HIGH

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #####  #    #   ##   #####  ######
//  #     #  #  #  #    # #    # #    #  #  #  #    # #
//  ####### #    # #    # #    # #    # #    # #    # #####
//  #     # ###### #####  #    # # ## # ###### #####  #
//  #     # #    # #   #  #    # ##  ## #    # #   #  #
//  #     # #    # #    # #####  #    # #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
WiFiClient espClient;
PubSubClient mqtt(espClient);

// LED Strip
CRGB currentLED[totalLEDs];

TaskHandle_t Task1;
TaskHandle_t Task2;

OneButton leftButton(leftButtonPin, true);
OneButton rightButton(rightButtonPin, true);
OneButton upButton(upButtonPin, true);
OneButton downButton(downButtonPin, true);
OneButton acceptButton(acceptButtonPin, true);

// * Screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// * ADC
Adafruit_ADS1115 ads;

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #   ##   #####  #      ######  ####
//  #     #  #  #  #    # #  #  #  #    # #      #      #
//  #     # #    # #    # # #    # #####  #      #####   ####
//   #   #  ###### #####  # ###### #    # #      #           #
//    # #   #    # #   #  # #    # #    # #      #      #    #
//     #    #    # #    # # #    # #####  ###### ######  ####
//
////////////////////////////////////////////////////////////////////////
int LEDBrightness = 25;  // As a percentage (saved as a dynamic variable to let us change later)

const char* wifiSsid = "I Don't Mind";
const char* wifiPassword = "Have2Biscuits";

const char* nodeName = "Uplight 1";

const char* disconnectMsg = "Uplight 1 Disconnected";

const char* mqttServerIP = "mqtt.kavanet.io";

// Wifi Params
bool WiFiConnected = false;
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

int mode = 5;

////////////////////////////////////////////////////////////////////////
//
//  ######                                                #####
//  #     # #####   ####   ####  #####    ##   #    #    #     # #####   ##   #####  ##### #    # #####
//  #     # #    # #    # #    # #    #  #  #  ##  ##    #         #    #  #  #    #   #   #    # #    #
//  ######  #    # #    # #      #    # #    # # ## #     #####    #   #    # #    #   #   #    # #    #
//  #       #####  #    # #  ### #####  ###### #    #          #   #   ###### #####    #   #    # #####
//  #       #   #  #    # #    # #   #  #    # #    #    #     #   #   #    # #   #    #   #    # #
//  #       #    #  ####   ####  #    # #    # #    #     #####    #   #    # #    #   #    ####  #
//
////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  // System architecture, dual core stuff
  xTaskCreatePinnedToCore(core1Loop, "Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);

  xTaskCreatePinnedToCore(core2Loop, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500);

  disableCore0WDT();  // This prevents the WDT taking out an idle core
  disableCore1WDT();  // the wifi code was triggering the WDT

  // LEDs
  FastLED.addLeds<NEOPIXEL, dataPin>(currentLED, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  // Wireless comms
  startWifi();
  startMQTT();
  startScreen();

  // On-board status led (Used for wifi and MQTT indication)
  pinMode(connectionLED, OUTPUT);
  pinMode(redPot, INPUT_PULLUP);
  pinMode(bluePot, INPUT_PULLUP);

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

  Serial << "\n|** " << nodeName << " **|" << endl;
  delay(100);
}

///////////////////////////////////////////////////////////////////////
//
//  #     #                    ######
//  ##   ##   ##   # #    #    #     # #####   ####   ####  #####    ##   #    #
//  # # # #  #  #  # ##   #    #     # #    # #    # #    # #    #  #  #  ##  ##
//  #  #  # #    # # # #  #    ######  #    # #    # #      #    # #    # # ## #
//  #     # ###### # #  # #    #       #####  #    # #  ### #####  ###### #    #
//  #     # #    # # #   ##    #       #   #  #    # #    # #   #  #    # #    #
//  #     # #    # # #    #    #       #    #  ####   ####  #    # #    # #    #
//
//////////////////////////////////////////////////////////////////////
void core1Loop(void* pvParameters) {
  for (;;) {
    // handleMQTT();
    handleWiFi();
    // delay(500);  // * Add this back if WDT issues come back
  }
}

void core2Loop(void* pvParameters) {
  for (;;) {
    upButton.tick();
    downButton.tick();
    leftButton.tick();
    rightButton.tick();
    acceptButton.tick();

    int lim = 250;
    int potMax = 3500;
    int red = 0;
    int blue = 0;
    int green = 0;
    int redPercentage, greenPercentage, bluePercentage;

    int redRaw = analogRead(redPot);
    int greenRaw = analogRead(greenPot);
    int blueRaw = analogRead(bluePot);

    if (redRaw < lim) {
      red = 0;
    } else if (redRaw > potMax) {
      red = potMax;
    } else {
      red = redRaw;
    }

    if (blueRaw < lim) {
      blue = 0;
    } else if (blueRaw > potMax) {
      blue = potMax;
    } else {
      blue = blueRaw;
    }

    if (greenRaw < lim) {
      green = 0;
    } else if (greenRaw > potMax) {
      green = potMax;
    } else {
      green = greenRaw;
    }

    redPercentage = map(red, 0, potMax, 0, 100);
    greenPercentage = map(green, 0, potMax, 0, 100);
    bluePercentage = map(blue, 0, potMax, 0, 100);

    int finalRed = map(redPercentage, 0, 100, 0, 255);
    int finalGreen = map(greenPercentage, 0, 100, 0, 255);
    int finalBlue = map(bluePercentage, 0, 100, 0, 255);

    Serial << "Red: " << finalRed << " "
           << "Green: " << finalGreen << " "
           << "Blue: " << finalBlue << endl
           << endl;

    for (int i = 0; i < totalLEDs; i++) {
      currentLED[i].setRGB(finalRed, finalGreen, finalBlue);
      // currentLED[i].setRGB(map(redPercentage, 0, 100, 0, 255), map(greenPercentage, 0, 100, 0, 255), map(bluePercentage, 0, 100, 0, 255));
    }

    FastLED.show();

    // if (WiFi.status() == WL_CONNECTED) {
    //   switch (mode) {
    //     case 0:  // Off
    //       FastLED.clear();
    //       FastLED.show();
    //       delay(5);
    //       break;

    //     case 1:
    //       fire.run(55, 120, 20, true);
    //       break;

    //     case 2:
    //       colourFade.run();
    //       break;

    //     case 3:
    //       colourCycle.run();
    //       break;

    //     case 4:
    //       meteorRain(0xff, 0xff, 0x00, 10, 64, true, 30);
    //       break;

    //     case 5:
    //       rainbow.run();
    //       // rainbowCycle(20);
    //       break;

    //     case 6:
    //       EVERY_N_MILLISECONDS(20) {
    //         pacifica_loop();
    //         FastLED.show();
    //         delay(5);
    //       }
    //       break;
    //   }
    delay(20);
  }
}

void loop() {
}