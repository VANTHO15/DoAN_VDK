#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define PIN_LED 2
#define PIN_CONFIG 14

int statusCode;
String content;


bool TestWifi();
void LaunchWeb();
void SetupAP();
bool LongPress();

ESP8266WebServer server(80);

void setup()
{
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
  Serial.println("Waiting.");
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }

}
///////////////////////////////////////////////////////////LOOP///////////////////////////////////////////////////////////
void loop() {
  if ((WiFi.status() == WL_CONNECTED))
  {
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(PIN_LED, HIGH);
      delay(1000);
      digitalWrite(PIN_LED, LOW);
      delay(1000);
    }
    if (LongPress())  // bấm nút config
    {
      for (int i = 0; i < 96; ++i)
      {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      Serial.println("Enter smartconfig");
    }
    for (int i = 0; i < 96; ++i)
    {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();

  }
  else
  {
    // code cho không có wifi

  }
}


//-------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change
bool TestWifi()
{
  int x = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( x < 20 )
  {
    if (WiFi.status() == WL_CONNECTED)
    {
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
  WiFi.softAP("HEARTBEAT & SPO2", "");
  Serial.println("softap");
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
    content += "<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" >";
    content += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css\">";
    content += "<style>";
    content += "body,";
    content += "html {";
    content += "margin: 0;";
    content += "padding: 0;";
    content += "height: 100%;";
    content += "background: #60a3bc !important;";
    content += "}";
    content += ".user_card {";
    content += "height: 400px;";
    content += "width: 350px;";
    content += "margin-top: auto;";
    content += "margin-bottom: auto;";
    content += "background: #f39c12;";
    content += "position: relative;";
    content += "display: flex;";
    content += "justify-content: center;";
    content += "flex-direction: column;";
    content += "padding: 10px;";
    content += "box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);";
    content += "-webkit-box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);";
    content += "-moz-box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);";
    content += "border-radius: 5px;";
    content += "}";
    content += ".brand_logo_container {";
    content += "position: absolute;";
    content += "height: 170px;";
    content += "width: 170px;";
    content += "top: -75px;";
    content += "border-radius: 50%;";
    content += "background: #60a3bc;";
    content += "padding: 10px;";
    content += "text-align: center;";
    content += "}";
    content += ".brand_logo {";
    content += "height: 150px;";
    content += "width: 150px;";
    content += "border-radius: 50%;";
    content += "border: 2px solid white;";
    content += "}";
    content += ".form_container {";
    content += "margin-top: 100px;";
    content += "}";
    content += ".login_btn {";
    content += "width: 100%;";
    content += "background: #c0392b !important;";
    content += "color: white !important;";
    content += "}";
    content += ".login_btn:focus {";
    content += "box-shadow: none !important;";
    content += "outline: 0px !important;";
    content += "}";
    content += ".login_container {";
    content += "padding: 0 2rem;";
    content += "}";
    content += ".input-group-text {";
    content += "background: #c0392b !important;";
    content += "color: white !important;";
    content += "border: 0 !important;";
    content += "border-radius: 0.25rem 0 0 0.25rem !important;";
    content += "}";
    content += ".input_user,";
    content += ".input_pass:focus {";
    content += "box-shadow: none !important;";
    content += "outline: 0px !important;";
    content += "}";
    content += ".custom-checkbox .custom-control-input:checked~.custom-control-label::before {";
    content += "background-color: #c0392b !important;";
    content += "}";
    content += " </style>";
    content += "</head>";
    content += " <body>";
    content += " <div class=\"container h-100\">";
    content += "<div class=\"d-flex justify-content-center h-100\">";
    content += "<div class=\"user_card\">";
    content += "<div class=\"d-flex justify-content-center\">";
    content += "<div class=\"brand_logo_container\">";
    content += "<img src=\"https://res.cloudinary.com/vantho15/image/upload/v1633134182/LoGo_s5cojv.png\" class=\"brand_logo\" alt=\"Logo\">";
    content += "</div>";
    content += "</div>";
    content += "<div class=\"d-flex justify-content-center form_container\">";
    content += "<form method='get' action='setting'>";
    content += "<div class=\"input-group mb-3\">";
    content += "<div class=\"input-group-append\">";
    content += "<span class=\"input-group-text\"><i class=\"fa fa-user\"></i></span>";
    content += "</div>";
    content += "<input name=\"ssid\" length=32 type=\"text\" class=\"form-control input_user\"  placeholder=\"Namewifi\">";
    content += "</div>";
    content += "<div class=\"input-group mb-2\">";
    content += "<div class=\"input-group-append\">";
    content += "<span class=\"input-group-text\"><i class=\"fa fa-key\"></i></span>";
    content += "</div>";
    content += "<input name='pass' length=64 type=\"password\" class=\"form-control input_pass\"  placeholder=\"Password\">";
    content += "</div>";
    content += "<div class=\"form-group\">";
    content += "<div class=\"custom-control custom-checkbox\">";
    content += "<input type=\"checkbox\" class=\"custom-control-input\" id=\"customControlInline\">";
    content += "<label class=\"custom-control-label\" for=\"customControlInline\">Remember me</label>";
    content += "</div>";
    content += "</div>";
    content += "<div class=\"d-flex justify-content-center mt-3 login_container\">";
    content += "<button type='submit'  class=\"btn login_btn\">Confirm</button>";
    content += "</div>";
    content += "</form>";
    content += "</div>";
    content += "</div>";
    content += "</div>";
    content += "</div>";
    content += "<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>";
    content += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script>";
    content += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script>";
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
