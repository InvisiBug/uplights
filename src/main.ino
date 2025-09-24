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
#include <ArduinoJson.h>  // Json Library
#include <EEPROM.h>
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
#define totalLEDs 9
// #define totalLEDs 60 // LEDs in the cloud

#define LED_BUILTIN 2  // ESP32, nothing required for ESP8266
#define connectionLED LED_BUILTIN

#define dataPin 23  // ESP32

#define upButtonPin 16
#define downButtonPin 17
#define leftButtonPin 0
#define rightButtonPin 12
#define acceptButtonPin 4

#define redPot 34
#define greenPot 35
#define bluePot 39

#define OFF LOW
#define ON HIGH

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

#define EEPROM_SIZE 1

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

// * Processor Core tasks
TaskHandle_t Task1;
TaskHandle_t Task2;

// * Buttons
OneButton leftButton(leftButtonPin, true);
OneButton rightButton(rightButtonPin, true);
OneButton upButton(upButtonPin, true);
OneButton downButton(downButtonPin, true);
OneButton acceptButton(acceptButtonPin, true);

// * Screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
int LEDBrightness = 100;  // As a percentage (saved as a dynamic variable to let us change later)

const char* wifiSsid = "Uplights";
const char* wifiPassword = "LetThereBeL1ght";

const char* nodeName = "Uplight 3";
const char* disconnectMsg = "Uplight 3 Disconnected";

const char* mqttServerIP = "uplights.kavanet.io";

// Wifi Params
bool WiFiConnected = false;
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

int menu = 0;  // Menu needs to start at 0 to prevent crashing when trying to draw to oled
int lastMenu = 0;
int page = 0;

int address = 0;
bool startup = true;

enum Menus { start,
             manual,
             off,
             remote,
             addr };

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

  //* System architecture, dual core stuff
  xTaskCreatePinnedToCore(core1Loop, "Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);

  xTaskCreatePinnedToCore(core2Loop, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500);

  disableCore0WDT();  // This prevents the WDT taking out an idle core
  disableCore1WDT();  // the wifi code was triggering the WDT

  //* LEDs
  FastLED.addLeds<NEOPIXEL, dataPin>(currentLED, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  //* Start hardware
  startWifi();
  startMQTT();
  startScreen();
  startButtons();

  //* Enable EEPROM
  EEPROM.begin(EEPROM_SIZE);
  address = EEPROM.read(0);
  Serial << "Address: " << address << endl;

  //* Enable pins
  pinMode(connectionLED, OUTPUT);
  pinMode(redPot, INPUT_PULLUP);
  pinMode(bluePot, INPUT_PULLUP);

  Serial << "\n|** " << nodeName << " **|" << endl;
  delay(100);
  menu = remote;
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
    handleMQTT();
    handleWiFi();
    if (startup) {
      delay(500);  // * Add this back if WDT issues come back
      startup = false;
    }
  }
}

void core2Loop(void* pvParameters) {
  for (;;) {
    yield();
    tickButtons();

    switch (menu) {
      case manual:
        middleText(F("Manual"));
        manualMode();
        lastMenu = menu;
        break;

      case off:
        middleText(F("Off"));
        if (lastMenu != menu) {
          FastLED.clear();
          FastLED.show();
        }
        lastMenu = menu;
        break;

      case remote:
        middleText(F("Remote"));
        if (lastMenu != menu) {
          FastLED.clear();
          FastLED.show();
        }
        lastMenu = menu;
        break;

      case addr:
        if (lastMenu != menu) {
          middleText(F("Address"));
          delay(500);
          FastLED.clear();
          FastLED.show();
        }

        middleText(String(address));

        lastMenu = menu;
        break;
    }
    delay(20);
  }
}

void loop() {
}
