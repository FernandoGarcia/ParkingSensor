void loop()
{
  esp8266React.loop();

  if (!WiFi.isConnected())
  {
    wifiConnected = false;
    // LOG(F("."));
  }
  else
  {
    wifiConnected = true;
  }

  if (!deviceConnected && oldDeviceConnected) // disconnecting
  {
    delay(500);                               // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();              // restart advertising
    LOGLN(F("Start advertising"));
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