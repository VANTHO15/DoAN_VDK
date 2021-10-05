#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#ifndef  APSSID
#define  APSSID "Van Tho 15"
#define  APPSK "vannhucu15"
#endif    
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
void handleRoot() 
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
    content += "<input name=\"ssid\" length=32 type=\"text\" class=\"form-control input_user\"  placeholder=\"Wifi\">";
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
}

void handletho()
{
  server.send(200, "text/html", "<h1>thotho</h1>");
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  //access point
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //Server config
  server.on("/", handleRoot);
  server.on("/tho", handletho);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(myIP);
}

void loop() 
{
  server.handleClient();
}
