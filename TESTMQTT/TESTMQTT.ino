#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "ngoinhaiot.com";
const char* mqtt_user = "Vantho15";
const char* mqtt_pass = "3759DFCEFE834F17";
int mqtt_port = 1111; // esp kết nối mqtt => TCP
String topicpub = "Vantho15/5C:CF:7F:39:22:AF"; // gửi dữ liệu

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Van Tho 15";
const char* pass = "vannhucu15";

String Data = "";
String ChuoiSendMQTT = "";
String nhietdo = "99";
String led = "OFF";
long last = 0;

void ConnectMqttBroker();
void reconnect();
void SendDataMQTT( String nhietdo ,  String led );

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  WiFi.mode(WIFI_STA);
  pinMode(D5, OUTPUT);
  WiFi.disconnect();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connect wifi");
  ConnectMqttBroker();
  Serial.println("Start ESP");
  last = millis();

}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (millis() - last >= 100)
  {
    SendDataMQTT(String(nhietdo), String(led));
    last = millis();
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
      Serial.println("Connected MQTT ngoinhaiot.com");
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
void SendDataMQTT( String nhietdo ,  String led ) // gửi data
{
  nhietdo = random(10,100);
  ChuoiSendMQTT = "";
  ChuoiSendMQTT = "{\"nhietdo\":\"" + String(nhietdo) + "\"," +
                  "\"led\":\"" + String(led) + "\"}";

  Serial.print("ChuoiSendMQTT:");
  Serial.println(ChuoiSendMQTT);
  client.publish(topicpub.c_str(), ChuoiSendMQTT.c_str());
}
