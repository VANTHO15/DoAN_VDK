#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "Van Tho 15";
const char *password = "vannhucu15";

const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  timeClient.setTimeOffset(+7*60*60);
}

void loop() {
  timeClient.update();

//  Serial.print(daysOfTheWeek[timeClient.getDay()]);
//  Serial.print(", ");
//  Serial.print(timeClient.getHours());
//  Serial.print(":");
//  Serial.print(timeClient.getMinutes());
//  Serial.print(":");
//  Serial.println(timeClient.getSeconds());
String Data = "A" +String( daysOfTheWeek[timeClient.getDay()]) + "B" + String(timeClient.getHours())+ "C" + String(timeClient.getMinutes())+
             "D" + String(timeClient.getSeconds())+ "E";
             Serial.println(Data);

  delay(1000);
}
