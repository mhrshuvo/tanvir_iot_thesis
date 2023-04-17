#include "DHT.h"
#define DHTTYPE DHT11
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define BOTtoken "61723160*62:AAECE2T5L9*SczAypTym1*efL76dU1s6nx8Gw"
#define CHAT_ID "-94856*8699*"
#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

ESP8266WebServer server(80);


const char* ssid = "Thesis";
const char* password = "10101010";


int Gas = D0;
int Ir = D1;
int alarm = D2;
const int DHTPin = 2;
DHT dht(DHTPin, DHTTYPE);

String s, f;

// Set your Static IP address
IPAddress local_IP(192, 168, 135, 182);
// Set your Gateway IP address
IPAddress gateway(192, 168, 135, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


void connectToWiFi() {
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void handleRoot() {
  if (digitalRead(Gas) == 0) {
    s = "\"Smoke Detected\"";
  }
  else {
    s = "\"NO Smoke Detected\"";
  }
  if (digitalRead(Ir) == 0) {
    f = "\"Fire Detected\"";
  } else {
    f = "\"NO Fire Detected\"";
  }

  String Value = "";
  Value += "{";

  Value += "\"UnitId\":\"Container 182\"";
  Value += ",";

  Value += "\"TempC\":";
  Value += dht.readTemperature();
  Value += ",";


  Value += "\"Humidity\":";
  Value +=  dht.readHumidity();
  Value += ",";

  Value += "\"Smoke\":";
  Value += s;
  Value += ",";

  Value += "\"Fire\":";
  Value += f;
  Value += ",";


  Value += "\"Name\":\"Omith Hasan\"";
  Value += ",";
  Value += "\"Id\":\"18103232\"";
  Value += "}";


  server.send(200, "application/json", Value); //--> Send web page
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  connectToWiFi();
  pinMode(Gas, INPUT);
  dht.begin();
  pinMode(Ir, INPUT);
  pinMode(alarm, OUTPUT);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif

  server.on("/", handleRoot);

  server.begin(); //--> Start server
  Serial.println("HTTP server started");

}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.print("Temperature:");
//  Serial.print(dht.readTemperature());
//  Serial.print(",");
//  Serial.print("Humidity:");
//  Serial.print(dht.readHumidity());
//  Serial.print(",");
//  Serial.print("Gas:");
//  Serial.println(digitalRead(Gas));
//  Serial.print("Fire:");
//  Serial.println(digitalRead(Ir));

  if (digitalRead(Gas) == 0) {
   bot.sendMessage(CHAT_ID, "Smoke is detected in container 182", "");
   tone(alarm, 100); 
  }
  if (digitalRead(Ir) == 0) {
    bot.sendMessage(CHAT_ID, "Flame is detected in container 182", "");
   tone(alarm, 100); 
  }
  if (Gas == 0 && Ir == 0) {
    bot.sendMessage(CHAT_ID, "Fire is detected in container 182", "");
    tone(alarm, 100); 
  }

  server.handleClient();

}
