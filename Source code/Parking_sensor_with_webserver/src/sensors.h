bool checkSensor()
{
  std::string msg;
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
      return false;
    }
  }

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
  }
  else if (pulse_bit[3] == 0)
  {
    Json[F("mode")] = "break";
  }

  if (tempID == sensorA)
  {
    Json[F("sensor")] = "A";
  }
  else if (tempID == sensorB)
  {
    Json[F("sensor")] = "B";
  }
  else if (tempID == sensorC)
  {
    Json[F("sensor")] = "C";
  }
  else if (tempID == sensorD)
  {
    Json[F("sensor")] = "D";
  }
  else if (tempID == sensorE)
  {
    Json[F("sensor")] = "E";
  }
  else if (tempID == sensorF)
  {
    Json[F("sensor")] = "F";
  }
  else if (tempID == sensorG)
  {
    Json[F("sensor")] = "G";
  }
  else if (tempID == sensorH)
  {
    Json[F("sensor")] = "H";
  }
  else
  {
    Json[F("sensor")] = "null";
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
  }
  else
  {
    Json[F("distance")] = distance;
  }


  if(Json[F("distance")] != "far")
  {
    if(checkDistance(Json[F("mode")], Json[F("distance")]))
    {
      #ifdef DEBUG
        debugMessages();
      #endif
      serializeJson(Json, msg);
      sendMessage(msg);
    }
  }
  return true;
}