// Tested with M5stack stamp C3. https://docs.m5stack.com/en/core/stamp_c3

#define BLUETOOTH_NAME "Parking Sensor"

#define DEBUG
#define PRINT_DISTANCE
#define PRINT_BIT
#define PRINT_DURATION

#define DATA_SIZE 18               // Amount of high pulses
#define BUTTON_DEFAULT_VALUE 400   // Default analog reading for GPIO 3 is around 560
#define MAX_MESSAGE_SIZE 100       // Max. message size for BLE

#define MAX_DIST_BREAK_SENSOR_G 90 // Ignore distances above this values
#define MAX_DIST_BREAK_SENSOR_H 90
#define MAX_DIST_BREAK_SENSOR_E 110
#define MAX_DIST_BREAK_SENSOR_F 110

#define MAX_DIST_REVERSE_SENSOR_A 190
#define MAX_DIST_REVERSE_SENSOR_B 190
#define MAX_DIST_REVERSE_SENSOR_C 190
#define MAX_DIST_REVERSE_SENSOR_D 190
#define MAX_DIST_REVERSE_SENSOR_G 150
#define MAX_DIST_REVERSE_SENSOR_H 150

#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266React.h>
#include <esp_coexist.h>

// Sensors order

//   G  E  F  H
//   4  5  6  7
//   ++++++++++
//     Front
//     Rear
//   +++++++++++
//   A  B  C  D
//   0  1  2  3

const int sensorA = 0;    // 000
const int sensorB = 1;    // 001
const int sensorC = 2;    // 010
const int sensorD = 3;    // 011
const int sensorE = 5;    // 101 - Changes the order from here.
const int sensorF = 6;    // 110
const int sensorG = 4;    // 100
const int sensorH = 7;    // 111

const byte pulsePin = 1;  // G1
const byte ledPin = 2;    // G2
const byte buttonPin = 3; // On board button

unsigned long pulse_time[DATA_SIZE];
byte pulse_bit[DATA_SIZE];
byte tempID;
String lastSensor;
byte lastDistance;

// UART service UUID
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800); // set number of LEDs, pin number, LED type.

AsyncWebServer server(80);
ESP8266React esp8266React(&server);

StaticJsonDocument <MAX_MESSAGE_SIZE> Json;

bool deviceConnected = false;
bool oldDeviceConnected = false;
byte ping;
bool wifiConnected;

#ifdef DEBUG
  #define LOG(...) Serial.print(__VA_ARGS__)
  #define LOGLN(...) Serial.println(__VA_ARGS__)
#else
  #define LOG(...)
  #define LOGLN(...)
#endif

#ifndef DEBUG
  #undef PRINT_DISTANCE
  #undef PRINT_BIT
  #undef PRINT_DURATION
#endif

void configBLE();
void sendMessage(std::string msg);
bool checkSensor();
void checkButton();
void ledStatus();
bool checkDistance(String mode, byte distance);
void debugMessages();

#include "bluetooth.h"
#include "misc.h"
#include "sensors.h"
#include "setup.h"
#include "loop.h"