void setup()
{
  // disableCore0WDT(); Long time inside a while will trigger the watchdog.

  esp_coex_preference_set(ESP_COEX_PREFER_BT); // Priority for bluetooth

  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  pinMode(pulsePin, INPUT);
  pinMode(buttonPin, INPUT);

  led.begin();

  configBLE();

  esp8266React.begin();
  server.begin();
}