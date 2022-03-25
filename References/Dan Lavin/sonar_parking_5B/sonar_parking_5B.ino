
/*
Information obtained:
18 bits total including start as data 0, defined a 0 value here
bits 0-3 always  0010
bit 4    0 for reverse 1 for not reverse
bit 5    0 for data present
bits 6-8:
    sensor A  000  0
    sensor B  001  1
    sensor C  010  2
    sensor D  011  3
    sensor E  101  5  note  change of sequence
    sensor F  110  6
    sensor G  100  4
    sensor H  111  7
    
bits 9-16  data  in meters
        samples:                    decimal
              .34 0010 0010     32 + 2  =34
              .35 0010 0011     32 + 3  =35
              .41 0010 1001     32 + 9  =41
              .42 0010 1010     32 + 10 =42
              .43 0010 1011     32 + 11 =43
              .44 0010 1100     32 + 12 =44
              .45 0010 1101     32 + 13 =45
              .57 0011 1001     32 + 16 + 9 = 57
             1.21 0111 1001     64 + 32 +16 + 9 = 121

bit 17 always 0
data pulse blocks sent every 40 ms
with reverse connected (yellow wire +12) 6 sensors are read ADBCGH
wirhout reverse 4 sensors are read EFGH
distances under 0.30 show as 0.0 in data
the display changes in 3cm increments, but dat changes in 1cm increments
off axis sensor response based on the display is about 32 degrees, not 
    checked against data stream


*/

int pin = 7;
int ledPin = 13;
#define DATA_SIZE 18
unsigned long pulse_length;
unsigned long pulse_set[DATA_SIZE];   //holds pulse lengthes in uS
byte pulse_value[DATA_SIZE];          // holds the binary equivalents
byte temp = 0;
byte distance = 0;


void setup()
{
pinMode(pin, INPUT);
pinMode(ledPin, OUTPUT);
Serial.begin(115200);
}

void loop(){
  
int i,m;
  //get information from pulses
for( i=0 ; i<DATA_SIZE ; i++){
pulse_length = pulseIn(pin, HIGH);
if (pulse_length > 1000) {           //start pulse
  pulse_set[0] = pulse_length;
}
  for( i=1 ; i<DATA_SIZE ; i++){
  pulse_set[i] =  pulseIn(pin, HIGH);
  if (pulse_set[i] < 100) {  pulse_value[i] = 0;}
   else pulse_value[i] = '?';
  if (pulse_set[i] > 200 && pulse_set[i] <250 ) pulse_value[i] = 1;
  }
}

//print it out
   // print as times
/*m=1;
for( i=0 ; i< DATA_SIZE ; i++){ 
    Serial.print(pulse_set[i],DEC);
  Serial.print(" ");
    if (m%4 == 0) Serial.print("    ");
  m++;
}
 Serial.println(" "); 
*/

  //print as bits
  /*
m=1;
for( i=0 ; i< DATA_SIZE ; i++){ 
    Serial.print(pulse_value[i],DEC);
   if (m ==4 || m == 6 || m == 9 || m == 13 || m == 17) Serial.print(" ");
  m++;
}

Serial.println("");
*/
//print as sensor id and distance -- valid data only

if (pulse_value[5] == 0 )       //data valid
      {
        byte sensor_id = 0;   //will hold sensor id bin
        char sensor_id_asc;   //will hold sensor id bin
        if (pulse_value[6]) bitSet(sensor_id,2);
        if (pulse_value[7]) bitSet(sensor_id,1);
        if (pulse_value[8]) bitSet(sensor_id,0);
        byte sensor_dist = 0;  //will hold sensor distance
        if (pulse_value[9]) bitSet(sensor_dist,7);
        if (pulse_value[10]) bitSet(sensor_dist,6);
        if (pulse_value[11]) bitSet(sensor_dist,5);
        if (pulse_value[12]) bitSet(sensor_dist,4);
        if (pulse_value[13]) bitSet(sensor_dist,3);
        if (pulse_value[14]) bitSet(sensor_dist,2);
        if (pulse_value[15]) bitSet(sensor_dist,1);
        if (pulse_value[16]) bitSet(sensor_dist,0);
        
        sensor_id_asc = sensor_id + 'A';  //now correct ascii value for ABCD and H
        if (sensor_id == 4) sensor_id_asc = 'G';
        if (sensor_id == 5) sensor_id_asc = 'E';
        if (sensor_id == 6) sensor_id_asc = 'F';
        Serial.print(sensor_id_asc);
        Serial.println(sensor_dist);
      } 
}


