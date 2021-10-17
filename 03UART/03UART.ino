#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#define PIN_LED 2
#define PIN_CONFIG 14
ESP8266WebServer server(80);
int statusCode;
String content;
bool TestWifi();
void LaunchWeb();
void SetupAP();
bool LongPress();                              // Config WIFI

#include "FirebaseESP8266.h"
#define FIREBASE_HOST "https://nhiptim-fee3b-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "cD4w8FSoWzgQa4S3rJZUNNxYS6IAOeFrnGN2uost"
FirebaseData firebaseData;
String path = "/";
long long last = 0;
int8_t NhipTim, Spo2, NhietDo , SetupFirebase = 0;
int32_t LedDo;
void WriteDataToFirebase();                      // FIREBASE

#include <PubSubClient.h>
const char* mqtt_server = "ngoinhaiot.com";
const char* mqtt_user = "Vantho15";
const char* mqtt_pass = "3759DFCEFE834F17";
int mqtt_port = 1111, Change = 0;     // esp kết nối mqtt => TCP
WiFiClient espClient;
PubSubClient client(espClient);
String ChuoiSendMQTT = "";
void ConnectMqttBroker();
void reconnect();
void SendDataMQTT(String NhipTim, String Spo2);   // MQTT

#include <SoftwareSerial.h>
const byte RX = D6;
const byte TX = D5;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
long lastUART = 0;
void Read_Uart();    // UART STM

#include <NTPClient.h>
#include <WiFiUdp.h>
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String DataToSTM, Thu, Gio, Phut, Giay, ThoiGian ;              // GET DATE TIME

void setup()
{
  timeClient.begin();
  timeClient.setTimeOffset(+7 * 60 * 60); // VietNam ở múi giờ thứ 7

  while (!Serial);
  mySerial.begin(115200);
  while (!mySerial);
  Serial.println("UART Start");
  lastUART = millis();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Ngat ket noi wifi da ket noi truoc do");
  WiFi.disconnect();
  EEPROM.begin(512); //Khởi tạo eeprom
  delay(10);
  pinMode(PIN_LED, OUTPUT); // GPIO2
  pinMode(PIN_CONFIG, INPUT); // GPIO14
  Serial.println();

  //Đọc ID và PASS từ EEPROM
  Serial.println("Reading EEPROM ssid");
  String esid = "";
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  int KiemTraUser = EEPROM.read(96);
  Serial.print("Firebase Kiem Tra: ");
  Serial.println(KiemTraUser);
  if (KiemTraUser == 255) // Chưa đăng kí User
  {
    EEPROM.write(96, 0);
  }
  EEPROM.commit();
  KiemTraUser = EEPROM.read(96);
  Serial.print("Firebase : ");
  Serial.println(KiemTraUser);

  // Kết nối vào wifi
  WiFi.begin(esid.c_str(), epass.c_str());
  if (TestWifi())
  {
    Serial.println("Succesfully Connected!!!");
    return;
  }
  else
  {
    LaunchWeb();
    SetupAP();
  }

  Serial.println();
  Serial.println("Waiting.");  // đợi có wifi thì thôi
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
}
///////////////////////////////////////////////////////////LOOP///////////////////////////////////////////////////////////
void loop()
{
  // Có wifi
  if ( EEPROM.read(96) == 0)  // Chỉ chạy 1 lần để đăng kí
  {
    WriteDataToFirebase();
  }
  digitalWrite(PIN_LED, 1);

  // GET DATE TIME
  timeClient.update();
  Thu = String(daysOfTheWeek[timeClient.getDay()]);
  Gio = String(timeClient.getHours());
  Phut = String(timeClient.getMinutes());
  Giay = String(timeClient.getSeconds());
  DataToSTM = "A" + Thu + "B" + Gio + "C" + Phut + "D" + Giay + "E\n" ;
  ThoiGian = Gio + ":" + Phut+ ":" + Giay;

  // READ/WRITE UART
  Read_Uart();
  if (millis() - lastUART >= 100)
  {
    Serial.println("Đã send : " + DataToSTM);
    mySerial.print(DataToSTM);
    lastUART = millis();
  }

  // NHẤN NÚT CONFIG
  if (LongPress())  // bấm nút config
  {
    for (int i = 0; i < 96; ++i)
    {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("Button Smartconfig");
  }

  // Connect MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // SEND TO MQTT
  if (millis() - last >= 100)
  {
    NhipTim = random(50, 100);
    Spo2 = random(30, 80);
    LedDo = random(100, 200);
    NhietDo = random(30, 80);
    SendDataMQTT(String(NhipTim), String(Spo2));
    last = millis();
  }
}
void Read_Uart()
{
  while (mySerial.available())
  {
    String st = mySerial.readString();
    Serial.println("Nhan : " + st);
  }
}

void ConnectMqttBroker()
{
  client.setServer(mqtt_server, mqtt_port); // set esp client kết nối MQTT broker
  delay(10);
}

void reconnect()
{
  while (!client.connected())
  {
    String clientId = "ESP8266Client123"; // tùy mình
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Connected MQTT HiveMQ");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
void SendDataMQTT(String NhipTim, String Spo2) // gửi data
{
  ChuoiSendMQTT = "";
  Change = ! Change;
  ChuoiSendMQTT = "{\"NhipTim\":\"" + String(NhipTim) + "\"," +
                  "\"Spo2\":\"" + String(Spo2) + "\","
                  "\"LedDo\":\"" + String(LedDo) + "\","
                  "\"NhietDo\":\"" + String(NhietDo) + "\","
                   "\"ThoiGian\":\"" + String(ThoiGian) + "\","
                  "\"Change\":\"" + String(Change) + "\"}";
  String MACID = WiFi.macAddress();
  String data = "Vantho15/" + MACID;
  client.publish(data.c_str(), ChuoiSendMQTT.c_str());
  Serial.print("Topic:    ");
  Serial.println(data);
  Serial.println(ChuoiSendMQTT);
}
void WriteDataToFirebase()
{
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("REASON:+ " + firebaseData.errorReason());
    Serial.println();
  }
  String MACID = WiFi.macAddress();
  Firebase.setString(firebaseData, path + "/" + MACID + "/ID" , "Admin");
  Firebase.setString(firebaseData, path + "/" + MACID + "/PASS", "Admin" );
  Serial.println("Dang Ki Firebase Thanh cong ID =Admin PASS =Admin " + MACID);

  EEPROM.write(96, 1);
  EEPROM.commit();
}
bool LongPress()
{
  static int lastPress = 0;
  if (millis() - lastPress > 3000 && digitalRead(PIN_CONFIG) == 0)
  {
    return true;
  } else if (digitalRead(PIN_CONFIG) == 1)
  {
    lastPress = millis();
  }
  return false;
}

bool TestWifi()
{
  int x = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( x < 20 )
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      // Nếu có wifi thì connect tới Brocker
      ConnectMqttBroker();
      last = millis();
      return true;
    }
    delay(500);
    Serial.print("*");
    x++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void LaunchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  CreateWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void SetupAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  WiFi.softAP("Heart Rate", "");
  LaunchWeb();
  Serial.println("over");
}

// 192.168.4.1
void CreateWebServer()
{
  server.on("/", []()
  {
    String content = "<html lang=\"en\"> ";
    content += "<head>";
    content += "<title>Config wifi</title>";
    content += "<meta charset=\"utf-8\">";
    content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">";
    content += "<style>";
    content += ".btn { display: inline-block; *display: inline; *zoom: 1; padding: 4px 10px 4px; margin-bottom: 0; font-size: 13px; line-height: 18px; color: #333333; text-align: center;text-shadow: 0 1px 1px rgba(255, 255, 255, 0.75); vertical-align: middle; background-color: #f5f5f5; background-image: -moz-linear-gradient(top, #ffffff, #e6e6e6); background-image: -ms-linear-gradient(top, #ffffff, #e6e6e6); background-image: -webkit-gradient(linear, 0 0, 0 100%, from(#ffffff), to(#e6e6e6)); background-image: -webkit-linear-gradient(top, #ffffff, #e6e6e6); background-image: -o-linear-gradient(top, #ffffff, #e6e6e6); background-image: linear-gradient(top, #ffffff, #e6e6e6); background-repeat: repeat-x; filter: progid:dximagetransform.microsoft.gradient(startColorstr=#ffffff, endColorstr=#e6e6e6, GradientType=0); border-color: #e6e6e6 #e6e6e6 #e6e6e6; border-color: rgba(0, 0, 0, 0.1) rgba(0, 0, 0, 0.1) rgba(0, 0, 0, 0.25); border: 1px solid #e6e6e6; -webkit-border-radius: 4px; -moz-border-radius: 4px; border-radius: 4px; -webkit-box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.2), 0 1px 2px rgba(0, 0, 0, 0.05); -moz-box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.2), 0 1px 2px rgba(0, 0, 0, 0.05); box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.2), 0 1px 2px rgba(0, 0, 0, 0.05); cursor: pointer; *margin-left: .3em; }";
    content += ".btn:hover, .btn:active, .btn.active, .btn.disabled, .btn[disabled] { background-color: #e6e6e6; }";
    content += ".btn-large { padding: 9px 14px; font-size: 15px; line-height: normal; -webkit-border-radius: 5px; -moz-border-radius: 5px; border-radius: 5px; }";
    content += ".btn:hover { color: #333333; text-decoration: none; background-color: #e6e6e6; background-position: 0 -15px; -webkit-transition: background-position 0.1s linear; -moz-transition: background-position 0.1s linear; -ms-transition: background-position 0.1s linear; -o-transition: background-position 0.1s linear; transition: background-position 0.1s linear; }";
    content += ".btn-primary, .btn-primary:hover { text-shadow: 0 -1px 0 rgba(0, 0, 0, 0.25); color: #ffffff; }";
    content += ".btn-primary.active { color: rgba(255, 255, 255, 0.75); }";
    content += ".btn-primary { background-color: #4a77d4; background-image: -moz-linear-gradient(top, #6eb6de, #4a77d4); background-image: -ms-linear-gradient(top, #6eb6de, #4a77d4); background-image: -webkit-gradient(linear, 0 0, 0 100%, from(#6eb6de), to(#4a77d4)); background-image: -webkit-linear-gradient(top, #6eb6de, #4a77d4); background-image: -o-linear-gradient(top, #6eb6de, #4a77d4); background-image: linear-gradient(top, #6eb6de, #4a77d4); background-repeat: repeat-x; filter: progid:dximagetransform.microsoft.gradient(startColorstr=#6eb6de, endColorstr=#4a77d4, GradientType=0);  border: 1px solid #3762bc; text-shadow: 1px 1px 1px rgba(0,0,0,0.4); box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.2), 0 1px 2px rgba(0, 0, 0, 0.5); }";
    content += ".btn-primary:hover, .btn-primary:active, .btn-primary.active, .btn-primary.disabled, .btn-primary[disabled] { filter: none; background-color: #4a77d4; }";
    content += ".btn-block { width: 100%; display:block; }";
    content += "* { -webkit-box-sizing:border-box; -moz-box-sizing:border-box; -ms-box-sizing:border-box; -o-box-sizing:border-box; box-sizing:border-box; }";
    content += "html { width: 100%; height:100%; overflow:hidden; }";
    content += "body {";
    content += "width: 100%;";
    content += "height:100%;";
    content += "font-family: 'Open Sans', sans-serif;";
    content += "background: #092756;";
    content += "background: -moz-radial-gradient(0% 100%, ellipse cover, rgba(104,128,138,.4) 10%,rgba(138,114,76,0) 40%),-moz-linear-gradient(top,  rgba(57,173,219,.25) 0%, rgba(42,60,87,.4) 100%), -moz-linear-gradient(-45deg,  #670d10 0%, #092756 100%);";
    content += "background: -webkit-radial-gradient(0% 100%, ellipse cover, rgba(104,128,138,.4) 10%,rgba(138,114,76,0) 40%), -webkit-linear-gradient(top,  rgba(57,173,219,.25) 0%,rgba(42,60,87,.4) 100%), -webkit-linear-gradient(-45deg,  #670d10 0%,#092756 100%);";
    content += "background: -o-radial-gradient(0% 100%, ellipse cover, rgba(104,128,138,.4) 10%,rgba(138,114,76,0) 40%), -o-linear-gradient(top,  rgba(57,173,219,.25) 0%,rgba(42,60,87,.4) 100%), -o-linear-gradient(-45deg,  #670d10 0%,#092756 100%);";
    content += "background: -ms-radial-gradient(0% 100%, ellipse cover, rgba(104,128,138,.4) 10%,rgba(138,114,76,0) 40%), -ms-linear-gradient(top,  rgba(57,173,219,.25) 0%,rgba(42,60,87,.4) 100%), -ms-linear-gradient(-45deg,  #670d10 0%,#092756 100%);";
    content += "background: -webkit-radial-gradient(0% 100%, ellipse cover, rgba(104,128,138,.4) 10%,rgba(138,114,76,0) 40%), linear-gradient(to bottom,  rgba(57,173,219,.25) 0%,rgba(42,60,87,.4) 100%), linear-gradient(135deg,  #670d10 0%,#092756 100%);";
    content += "filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#3E1D6D', endColorstr='#092756',GradientType=1 );";
    content += "}";
    content += ".login {";
    content += "position: absolute;";
    content += "top: 50%;";
    content += "left: 50%;";
    content += "margin: -150px 0 0 -150px;";
    content += "width:300px;";
    content += "height:300px;";
    content += "}";
    content += ".login h1 {color: aqua; text-shadow: 0 0 10px rgba(0,0,0,0.3); letter-spacing:1px; text-align:center; }";
    content += "input {";
    content += "width: 100%; ";
    content += "margin-bottom: 10px; ";
    content += "background: rgba(0,0,0,0.3);";
    content += "border: none;";
    content += "outline: none;";
    content += "padding: 10px;";
    content += "font-size: 13px;";
    content += "color: #fff;";
    content += "text-shadow: 1px 1px 1px rgba(0,0,0,0.3);";
    content += "border: 1px solid rgba(0,0,0,0.3);";
    content += "border-radius: 4px;";
    content += "box-shadow: inset 0 -5px 45px rgba(100,100,100,0.2), 0 1px 1px rgba(255,255,255,0.2);";
    content += "-webkit-transition: box-shadow .5s ease;";
    content += "-moz-transition: box-shadow .5s ease;";
    content += "-o-transition: box-shadow .5s ease;";
    content += "-ms-transition: box-shadow .5s ease;";
    content += "transition: box-shadow .5s ease;";
    content += "}";
    content += "input:focus { box-shadow: inset 0 -5px 45px rgba(100,100,100,0.4), 0 1px 1px rgba(255,255,255,0.2); }";
    content += "</style>";
    content += "</head>";
    content += "<body>";
    content += "<div class=\"login\">";
    content += "<h1>SETTING WIFI</h1>";
    content += "<form method='get' action='setting'>";
    content += "<input name='ssid' length=32 type=\"text\" placeholder=\"UserName\" required=\"required\" />";
    content += "<input name='pass' length=64 type=\"password\" placeholder=\"Password\" required=\"required\" />";
    content += "<button type=\"submit\" class=\"btn btn-primary btn-block btn-large\">Confirm</button>";
    content += "</form>";
    content += "</div>";
    content += "</body>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

  server.on("/setting", []() {
    String qsid = server.arg("ssid");
    String qpass = server.arg("pass");
    if (qsid.length() > 0 && qpass.length() > 0)
    {
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      Serial.println(qsid);
      Serial.println("");
      Serial.println(qpass);
      Serial.println("");

      Serial.println("writing eeprom ssid:");
      for (int i = 0; i < qsid.length(); ++i)
      {
        EEPROM.write(i, qsid[i]);
        Serial.print("Wrote: ");
        Serial.println(qsid[i]);
      }
      Serial.println("writing eeprom pass:");
      for (int i = 0; i < qpass.length(); ++i)
      {
        EEPROM.write(32 + i, qpass[i]);
        Serial.print("Wrote: ");
        Serial.println(qpass[i]);
      }
      EEPROM.commit();

      content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
      statusCode = 200;
      ESP.reset();
    }
    else
    {
      content = "{\"Error\":\"404 not found\"}";
      statusCode = 404;
      Serial.println("Sending 404");
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(statusCode, "application/json", content);
  });
}
