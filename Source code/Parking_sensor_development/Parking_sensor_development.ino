#define PRINT_DURATION
#define PRINT_BIT
#define PRINT_DISTANCE

#define DATA_SIZE 18 // Amount of high pulses

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

const int pin = D1;
const int ledPin = 2;
unsigned long pulse_time[DATA_SIZE];   // Holds pulse lengthes in uS
byte pulse_bit[DATA_SIZE];             // Holds the binary equivalents

// *** NodeMCU PINS ***
/*
  #define D0    16 // Has LED
  #define D1    5  // PWM e SCL
  #define D2    4  // PWM e SDA
  #define D3    0  // PWM
  #define D4    2  // PWM - Has LED
  #define D5    14 // CLK e PWM
  #define D6    12 // MISO e PWM
  #define D7    13 // MOSI e PWM
  #define D8    15 // CS e PWM
  #define D9    3  // RX
  #define D10   1  // TX
*/

void setup()
{
  pinMode(pin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  byte tempID;
  byte distance;
  int temp;
  unsigned long pulse_length = pulseIn(pin, HIGH);

  pulse_time[0] = pulse_length;
  pulse_bit[0] = 1;

  while (pulse_length < 1500) // Wait for the first pulse.
  {
    pulse_length = pulseIn(pin, HIGH);
    pulse_time[0] = pulse_length;
    //Serial.println(pulse_length); // Debug
  }

  Serial.println("\n------ New loop ------");

  for ( int i = 1 ; i < DATA_SIZE ; i++)
  {
    pulse_time[i] =  pulseIn(pin, HIGH);

    if (pulse_time[i] < 300)
    {
      pulse_bit[i] = 0;
    }
    else
    {
      pulse_bit[i] = 1;
    }
  }

  // Set all bits to zero befor start
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
    Serial.println("Reverse mode selected.");
  }
  else if (pulse_bit[3] == 0)
  {
    Serial.println("Break mode selected.");
  }

#ifdef PRINT_DURATION
  for ( int i = 0 ; i < DATA_SIZE ; i++)
  {
    Serial.print(pulse_time[i], DEC);
    Serial.print(" ");
  }
  Serial.println();
#endif

#ifdef PRINT_BIT
  for ( int i = 0 ; i < DATA_SIZE ; i++)
  {
    Serial.print(pulse_bit[i]);
    if ((i == 3) || (i == 4) || (i == 5) || (i == 8) || (i == 16))
    {
      Serial.print(" ");
    }
  }
  Serial.println();
#endif

#ifdef PRINT_DISTANCE

  if (tempID == sensorA)
  {
    Serial.print("Sensor A = ");
  }
  else if (tempID == sensorB)
  {
    Serial.print("Sensor B = ");
  }
  else if (tempID == sensorC)
  {
    Serial.print("Sensor C = ");
  }
  else if (tempID == sensorD)
  {
    Serial.print("Sensor D = ");
  }
  else if (tempID == sensorE)
  {
    Serial.print("Sensor E = ");
  }
  else if (tempID == sensorF)
  {
    Serial.print("Sensor F = ");
  }
  else if (tempID == sensorG)
  {
    Serial.print("Sensor G = ");
  }
  else if (tempID == sensorH)
  {
    Serial.print("Sensor H = ");
  }
  else
  {
    Serial.println("Wrong sensor ID found.");
    Serial.print("Sensor ID found = ");
    Serial.println(temp, DEC);
    /*Serial.print("ID sensor A = ");
      Serial.println(sensorA, DEC);
      Serial.print("ID sensor B = ");
      Serial.println(sensorB, DEC);
      Serial.print("ID sensor C = ");
      Serial.println(sensorC, DEC);
      Serial.print("ID sensor D = ");
      Serial.println(sensorD, DEC);*/
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
    Serial.println("So far!");
  }
  else
  {
    Serial.print(distance, DEC);
    Serial.println(" cm");
  }
#endif
}
