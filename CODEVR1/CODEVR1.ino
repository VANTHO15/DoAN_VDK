#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Van Tho 15";
const char* password = "vannhucu15";

const char* awsEndpoint = "a35cho9o6i7esh-ats.iot.us-east-1.amazonaws.com";


// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUIoBd1YJrNCEjZO2lgqJoEhpEKHcwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDkyODIzNDI0
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK9lkiljdh0FTah6WZXn
Fv5/BAdxl7lBr03ZET5CNHo1OkB6aOTtR+3Ycaq6gB9yfNYchYOjy2GtgaQYnCJz
WGYyzrJM6SiHFlTu+3/LDYcyLT++EqkXgruimpyy976NIH8sUUgr9vLQzY9yPKh2
NQCjVM/s9RGUuFF/y8eKTagZg0eUWyER2x5qhL37u+ieTkwozF6y4RdHwD2ETaNr
GnHgAhCGIjLBKs/YCQKCHY5Q/fMuoWXcqWA61lwUn07HFrUQjFFAGRKblzaXffak
geWYnNMakRvG88f29D57sa3KigWFGLwL9p69p+OPOKkstVaSvvE5Sna5+gSziJbh
Bp8CAwEAAaNgMF4wHwYDVR0jBBgwFoAU9iovQ5D77F0MQj88gD9SAYUdWoIwHQYD
VR0OBBYEFDDdQr9YG8CAmoujgRORqZcQdaIDMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCYC5/7hndls1sZgRgY82zAPRMy
5sutL3+KUQ1+XOWKgT3jN1m8NamZWeR8j8Fpd+IDxPq6Vpt/owu8Tj/hhKeTVYzc
wNEpp+P8WkX1gZbyYOre/W67zc6nXh2aZPOZNjttbMteDYDEcsnpng07tYNjnFXn
L/y/WjpsPi2RBbGG+T3nQDDFOKxin0/YvEQG9MP+aeuE9fy/K8oUK0miVtegYW7B
Tzt7ACYGBfEDF+AsoaTXexegHLtpKeSwqmen5E+OMhcVPqqbCkEyid0nuH3saN0i
oPcpZezPu7vvG+C+W5I3D8FRUu9EBtECIQ3V/taPazVcYsr/zbjwPXCzRJ/V
-----END CERTIFICATE-----
)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAr2WSKWN2HQVNqHpZlecW/n8EB3GXuUGvTdkRPkI0ejU6QHpo
5O1H7dhxqrqAH3J81hyFg6PLYa2BpBicInNYZjLOskzpKIcWVO77f8sNhzItP74S
qReCu6KanLL3vo0gfyxRSCv28tDNj3I8qHY1AKNUz+z1EZS4UX/Lx4pNqBmDR5Rb
IRHbHmqEvfu76J5OTCjMXrLhF0fAPYRNo2saceACEIYiMsEqz9gJAoIdjlD98y6h
ZdypYDrWXBSfTscWtRCMUUAZEpuXNpd99qSB5Zic0xqRG8bzx/b0PnuxrcqKBYUY
vAv2nr2n4484qSy1VpK+8TlKdrn6BLOIluEGnwIDAQABAoIBAQCWadpgWAVQed3R
H6t33AdiFHbWE1nz6xp0/jPFWm06aC8eYFtYBwxqMfW4XdvGHB5mV2tAKtUdXPgS
S3CKMrSaX/PWABLrSH2kNJXFT9K4IBWh1lSWYa0hUB6bRvcimFBH/y9tXxaVwO6n
l2Xhag2mEGuJLEI14331lkyhQD+8aUkgzrZ7zPclpw9yO0lX8gc6qH1r73771v+3
Ko+tCGyWqQmahV90o8aAkX949CbSWxVR+KoyjzPviQHIRHFMQ/gEsI0ckmsQpok/
Ed1YuQ4ApQnMs2hKiCCZoeBqDw+mCnVig5XfRGFvv40O2+5Myyb6/pqjGnbp24HI
yoVP/1bxAoGBANhfS9hBd4pYjePMeVADatsjwSgJ/yrG0am2hZSVy5NGpm0dEaxF
fEUNhrjWc/CPvZuSJmjhg9cz5PCLJRnEfEpDN2Gl6jmyagb0Bd3XujMnkc8/mvsZ
jknsz04/s4JzRPgXlWblBpMabDvLOJm1tHerRBgvFmSb8Or44tum4iddAoGBAM+F
HyVduV3/b6G2PIpmKjmkr0hWEHdJBxEGLawExuKJg90RVyK7+ntaTLi3Yqdd4DXa
ipMu5d5p5YduSE9Ey5zXjLnBP0NhY4qawfYSJnwjhoFDToPX959AzVKVnAVDT/qB
/M+y2Bczq//P0XttiXpgpMLBM7uqLoAN5mJaAXIrAoGBAJv4henLt2ooz49oNAKz
k/GH4pMEMcYaL8TQ55XmLStrAEkjbTvrktsMFxNis4yzcRCV184+0hTtOZkUFJhw
hM/biC6BtUgGDlwYx6nU+kopuffVd9iN4X30kLNtQ+9DPHJCPdZAPe8IWwD3S0Oy
MfcZpCpytwerSY13/8AqmleZAoGAN9zUF5D1V3JtsJCIF5GNCFtmvAg/An1sI48Q
OGBI0a7HCnmihT0pHo+eW5MyQW+p7ZVY/GoBgy/qk9yCJqSUUzJLNa3pw8+xphwm
3oDgT0aY7K8bsa4vgHVHUFXp3BjUrNDVJ2wzc3t7b6+X6qv/+CuP7sVnkbssLViQ
jiVAgmUCgYBVM4SVWbWOQiO9vvSorECmaSItRPi8mRKLFn5n1VmG+lK791AkAjJN
0EfZBIUyNHhru+cXsl5bMJA2zgOguGkU5T1E4/P2PxYcxnG08GnH6M3Sn4p+gkzN
gPCyeSZlp+R1xYRTKIYo0bL3+CDiwYpIkAW4yhk/6YDWn2pumxIyMA==
-----END RSA PRIVATE KEY-----
)EOF";

// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
  
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;
    pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
