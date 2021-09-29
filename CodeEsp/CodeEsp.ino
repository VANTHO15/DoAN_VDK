#include <SoftwareSerial.h> //2.5.0
const byte RX = D6;
const byte TX = D5;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);

long last = 0;
void setup()
{
  Serial.begin(9600);
  while (!Serial);
  mySerial.begin(9600);
  while (!mySerial);
  Serial.println("Start");

  last = millis();
}

void loop()
{
  Read_Uart();
  if(millis() - last >= 1000)
  {
    Serial.println("Đã send lệnh");
    mySerial.println("a");
    last = millis();
  }
}

void Read_Uart()
{
  while (mySerial.available())
  {
    String st = mySerial.readString();
    Serial.println("Nhan : "+ st);
  }
}
