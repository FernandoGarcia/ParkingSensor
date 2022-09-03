bool checkDistance(String mode, byte distance)
{
  if(mode == "break")
  {
    if((distance <= MAX_DIST_BREAK_SENSOR_G) ||
       (distance <= MAX_DIST_BREAK_SENSOR_H) ||
       (distance <= MAX_DIST_BREAK_SENSOR_E) ||
       (distance <= MAX_DIST_BREAK_SENSOR_F))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if(mode == "reverse")
  {
    if((distance <= MAX_DIST_REVERSE_SENSOR_A) ||
       (distance <= MAX_DIST_REVERSE_SENSOR_B) ||
       (distance <= MAX_DIST_REVERSE_SENSOR_C) ||
       (distance <= MAX_DIST_REVERSE_SENSOR_D) ||
       (distance <= MAX_DIST_REVERSE_SENSOR_G) ||
       (distance <= MAX_DIST_REVERSE_SENSOR_H))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}
void checkButton()
{
  if (analogRead(buttonPin) < BUTTON_DEFAULT_VALUE)
  {
    LOGLN(F("Resetting credentials..."));
    delay(5000UL); // For debug
    esp8266React.factoryReset();
  }
}

void ledStatus()
{
  byte ledIndex = 0;

  led.setBrightness(255);

  if((deviceConnected == true) && (wifiConnected == false))
  {
    led.setPixelColor(ledIndex, 0, 0, 255); // Blue
  }
  else if((deviceConnected == true) && (wifiConnected == true))
  {
    led.setPixelColor(ledIndex, 255, 0, 255); // violet
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

void debugMessages()
{
  #ifdef PRINT_DURATION
    for ( int i = 0 ; i < DATA_SIZE ; i++)
    {
      LOG(pulse_time[i], DEC);
      LOG(F(" "));
    }
    LOGLN();
  #endif

  #ifdef PRINT_BIT
    for ( int i = 0 ; i < DATA_SIZE ; i++)
    {
      LOG(pulse_bit[i]);
      if ((i == 3) || (i == 4) || (i == 5) || (i == 8) || (i == 16))
      {
        LOG(F(" "));
      }
    }
    LOGLN();
  #endif

  #ifdef PRINT_DISTANCE
    #if !defined(PRINT_BIT) || !defined(PRINT_DURATION)
      LOGLN("");
    #endif

    if (Json[F("mode")] == "reverse")
    {
      LOGLN(F("Reverse mode selected."));
    }
    else if (Json[F("mode")] == "break")
    {
      LOGLN(F("Break mode selected."));
    }

    if(Json[F("sensor")] == "null")
    {
      LOGLN(F("Wrong sensor ID found."));
      LOG(F("Sensor ID found = "));
      LOGLN(tempID, DEC);
    }
    else
    {
      LOG(F("Sensor "));
      LOG(Json[F("sensor")].as<String>());
      LOG(F(" = "));
    }

    if (Json[F("distance")] == "far")
    {
      LOGLN(F("So far!"));
    }
    else
    {
      LOG(Json[F("distance")].as<int>());
      LOGLN(F(" cm."));
    }
  #endif
}