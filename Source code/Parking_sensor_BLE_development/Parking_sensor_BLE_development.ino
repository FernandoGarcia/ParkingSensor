// Tested with M5stack stamp C3. https://docs.m5stack.com/en/core/stamp_c3

#define BLUETOOTH_NAME "Parking Sensor"

#define DEBUG
#define DATA_SIZE 18 // Amount of high pulses
#define BUTTON_DEFAULT_VALUE 400 // Default analog reading for GPIO 3 is around 560
#define MAX_MESSAGE_SIZE 100 // Max. message size for BLE

#include <WiFi.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

// Sensors order

//   G  E  F  H
//   4  5  6  7
//   ++++++++++
//     Front
//     Rear
//   +++++++++++
//   A  B  C  D
//   0  1  2  3

const int sensorA = 0; // 000
const int sensorB = 1; // 001
const int sensorC = 2; // 010
const int sensorD = 3; // 011
const int sensorE = 5; // 101- Changes the order from here.
const int sensorF = 6; // 110
const int sensorG = 4; // 100
const int sensorH = 7; // 111

const byte pulsePin = 1;  // G1
const byte ledPin = 2;    // G2
const byte buttonPin = 3; // On board button

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800); //set number of LEDs, pin number, LED type.

bool deviceConnected = false;
bool oldDeviceConnected = false;
byte ping;
bool wifiConnected;
char ssid[] = "Nossarede";
char pass[] = "verysafenetwork";

void configBLE();
void sendMessage(std::string msg);
bool checkSensor();
void checkButton();
void ledStatus();
  
#ifdef DEBUG
#define LOG(...) Serial.print(__VA_ARGS__)
#define LOGLN(...) Serial.println(__VA_ARGS__)
#else
#define LOG(...)
#define LOGLN(...)
#endif

void setup()
{
  //disableCore0WDT(); Long time inside a while will trigger the watchdog.

#ifdef DEBUG
  Serial.begin(115200);
#endif

  pinMode(pulsePin, INPUT);
  pinMode(buttonPin, INPUT);

  led.begin();
  
  configBLE();

  WiFi.begin(ssid, pass);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnected = false;
    LOG(F("."));
  }
  else
  {
    wifiConnected = true;
  }

  if (!deviceConnected && oldDeviceConnected) // disconnecting
  {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    LOGLN(F("start advertising"));
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) // connecting
  {
    oldDeviceConnected = deviceConnected;
  }
  checkSensor();
  checkButton();
  ledStatus();
}

class MyServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
      deviceConnected = true;
      LOGLN(F("Device connected!"));
    }

    void onDisconnect(BLEServer* pServer)
    {
      deviceConnected = false;
      LOGLN(F("Device disconnected!"));
    }
};

void configBLE()
{
  BLEDevice::init(BLUETOOTH_NAME); // Create the BLE Device
  BLEDevice::setMTU(MAX_MESSAGE_SIZE); // debug with larger MTU size
  pServer = BLEDevice::createServer(); // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks()); // Set Callback

  BLEService *pService = pServer->createService(SERVICE_UUID); // Create the BLE Service

  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY); // Create a BLE Characteristic

  pTxCharacteristic->addDescriptor(new BLE2902()); // Add descriptor

  pService->start(); // Start the service

  pServer->getAdvertising()->start();// Start advertising
}

bool checkSensor()
{
  DynamicJsonDocument Json(MAX_MESSAGE_SIZE);

  unsigned long pulse_time[DATA_SIZE];
  byte pulse_bit[DATA_SIZE];
  std::string msg;
  byte tempID;
  byte distance;
  int temp;
  int count = 0;
  unsigned long pulse_timeout = 30000;
  String message;

  unsigned long pulse_length = pulseIn(pulsePin, HIGH, pulse_timeout);


  pulse_time[0] = pulse_length;
  pulse_bit[0] = 1;

  while (pulse_length < 1500) // Wait for the first pulse.
  {
    pulse_length = pulseIn(pulsePin, HIGH, pulse_timeout);

    pulse_time[0] = pulse_length;
    
    if (count++ > DATA_SIZE)
    {
      if(ping++ > 10)
      {
        ping = 0;
        message = "{\"ping\":";
        message += millis() / 1000;
        message += "}";
        sendMessage(message.c_str());
      }
      //LOGLN(F("Return"));
      return false;
    }
  }

  LOGLN(F("\n------ New loop ------"));

  for ( int i = 1 ; i < DATA_SIZE ; i++)
  {
    pulse_time[i] =  pulseIn(pulsePin, HIGH, pulse_timeout);

    if (pulse_time[i] < 300)
    {
      pulse_bit[i] = 0;
    }
    else
    {
      pulse_bit[i] = 1;
    }
  }

  // Set all bits to zero before start
  bitWrite(tempID, 0, 0);
  bitWrite(tempID, 1, 0);
  bitWrite(tempID, 2, 0);
  bitWrite(tempID, 3, 0);
  bitWrite(tempID, 4, 0);
  bitWrite(tempID, 5, 0);
  bitWrite(tempID, 6, 0);
  bitWrite(tempID, 7, 0);

  for (int i = 6; i <= 8; i++)
  {
    if (pulse_bit[i] == 1)
    {
      bitSet(tempID, 8 - i);
    }
  }

  if (pulse_bit[4] == 0)
  {
    Json[F("mode")] = "reverse";
    LOGLN(F("Reverse mode selected."));
  }
  else if (pulse_bit[3] == 0)
  {
    Json[F("mode")] = "break";
    LOGLN(F("Break mode selected."));
  }

  for ( int i = 0 ; i < DATA_SIZE ; i++)
  {
    LOG(pulse_time[i], DEC);
    LOG(F(" "));
  }
  LOGLN();

  for ( int i = 0 ; i < DATA_SIZE ; i++)
  {
    LOG(pulse_bit[i]);
    if ((i == 3) || (i == 4) || (i == 5) || (i == 8) || (i == 16))
    {
      LOG(F(" "));
    }
  }
  LOGLN();

  if (tempID == sensorA)
  {
    Json[F("sensor")] = "a";
    LOG(F("Sensor A = "));
  }
  else if (tempID == sensorB)
  {
    Json[F("sensor")] = "b";
    LOG(F("Sensor B = "));
  }
  else if (tempID == sensorC)
  {
    Json[F("sensor")] = "c";
    LOG(F("Sensor C = "));
  }
  else if (tempID == sensorD)
  {
    Json[F("sensor")] = "d";
    LOG(F("Sensor D = "));
  }
  else if (tempID == sensorE)
  {
    Json[F("sensor")] = "e";
    LOG(F("Sensor E = "));
  }
  else if (tempID == sensorF)
  {
    Json[F("sensor")] = "f";
    LOG(F("Sensor F = "));
  }
  else if (tempID == sensorG)
  {
    Json[F("sensor")] = "g";
    LOG(F("Sensor G = "));
  }
  else if (tempID == sensorH)
  {
    Json[F("sensor")] = "h";
    LOG(F("Sensor H = "));
  }
  else
  {
    Json[F("sensor")] = "null";
    LOGLN(F("Wrong sensor ID found."));
    LOG(F("Sensor ID found = "));
    LOGLN(temp, DEC);
  }

  for (int i = 16; i >= 9; i--)
  {
    if (pulse_bit[i] == 1)
    {
      bitSet(distance, 16 - i);
    }
  }

  if (distance == 255)
  {
    Json[F("distance")] = "far";
    LOGLN(F("So far!"));
  }
  else
  {
    Json[F("distance")] = distance;
    LOG(distance, DEC);
    LOGLN(F(" cm"));
  }

  serializeJson(Json, msg);

  sendMessage(msg);
  return true;
}

void sendMessage(std::string msg)
{
  if (deviceConnected == true)
  {
    pTxCharacteristic->setValue(msg);
    pTxCharacteristic->notify();

    LOG(F("Message = "));
    LOGLN(msg.c_str());
  }
  else
  {
    LOGLN(F("Not connected!"));
    //pServer->getAdvertising()->start();// Start advertising
  }
}

void checkButton()
{
  if (analogRead(buttonPin) < BUTTON_DEFAULT_VALUE)
  {
    LOGLN(F("Resetting credentials..."));
    delay(60000UL); // For debug
  }
}

void ledStatus()
{
  byte ledIndex = 0;
  
  led.setBrightness(255);
  
  if(deviceConnected == true)
  {
    led.setPixelColor(ledIndex, 0, 0, 255); // Blue
  }
  else if((deviceConnected == false) && (wifiConnected == true))
  {
    led.setPixelColor(ledIndex, 0, 255, 0); // Green
  }
  else
  {
    led.setPixelColor(ledIndex, 255, 0, 0); // Red
  }  
  led.show();
}
