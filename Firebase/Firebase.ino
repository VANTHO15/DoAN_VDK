#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

#define FIREBASE_HOST "https://nhiptim-fee3b-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "cD4w8FSoWzgQa4S3rJZUNNxYS6IAOeFrnGN2uost"
#define WIFI_SSID "Van Tho 15"
#define WIFI_PASSWORD "vannhucu15"

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;
long long last = 0;
int8_t NhipTim, Spo2;

void ReadDataToFirebase();
void WriteDataToFirebase();
void setup()
{
  Serial.begin(9600);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ") ;
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("REASON:+ " + firebaseData.errorReason());
    Serial.println();
  }
  
  last = millis();
}

void loop()
{
  WriteDataToFirebase();
}

void WriteDataToFirebase()
{
  if (millis() - last > 500)
  {
    NhipTim = random(50, 100);
    Spo2 = random(30, 80);
    Firebase.setString(firebaseData, path + "/Cua/NhipTim",String(NhipTim));
    Firebase.setString(firebaseData, path + "/Cua/Spo2",String(Spo2) );
    last = millis();
  }
}
