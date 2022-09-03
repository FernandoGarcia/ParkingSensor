class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    deviceConnected = true;
    LOGLN(F("Bluetooth connected!"));
  }

  void onDisconnect(BLEServer* pServer)
  {
    deviceConnected = false;
    LOGLN(F("Bluetooth disconnected!"));
  }
};

void configBLE()
{
  BLEDevice::init(BLUETOOTH_NAME);                             // Create the BLE Device
  BLEDevice::setMTU(MAX_MESSAGE_SIZE);                         // debug with larger MTU size
  pServer = BLEDevice::createServer();                         // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks());              // Set Callback

  BLEService *pService = pServer->createService(SERVICE_UUID); // Create the BLE Service
  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902()); // Add descriptor

  pService->start();                               // Start the service

  pServer->getAdvertising()->start();              // Start advertising

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P18);
}

void sendMessage(std::string msg)
{
  if (deviceConnected == true)
  {
    if((lastSensor != Json[F("sensor")]) || (lastDistance != Json[F("distance")]))
    {
      lastSensor = Json[F("sensor")].as<String>();
      lastDistance = Json[F("distance")].as<int>();

      pTxCharacteristic->setValue(msg);
      pTxCharacteristic->notify();

      LOG(F("Message = "));
      LOGLN(msg.c_str());
      LOGLN("");
      delay(random(70, 100)); // Give some time for bluetooth send the message
    }
  }
  else
  {
    LOGLN(F("Bluetooth not connected!\n"));
  }
}