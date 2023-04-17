#include <Arduino_JSON.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "Thesis";
const char* password = "10101010";

char* SROOT = "http://192.168.135.181";
char* SROOTo = "http://192.168.135.182";


String Root,Rooto ;
const char *Uid, *smoke,*fire,*Uido, *smokeo,*fireo;

double temperature, humidity,temperatureo, humidityo;

unsigned long previousMillis = 0;
const long interval = 5000;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");
  delay(500);
  Serial.println("Initializing...");
  delay(1000);


  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  lcd.setCursor(0, 0);
  lcd.print("Connected to WiFi");
  Serial.println(WiFi.localIP());
  delay(500);
  lcd.clear();


}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {

      callData(SROOT);
      callData(SROOTo);
      
    }
  }

}


void callData(char s[]){
   Root =  httpGETRequest(s);
      Serial.println(Root);
      JSONVar myObject = JSON.parse(Root);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      JSONVar keys = myObject.keys();

      JSONVar valueT =  myObject[keys[1]];
      JSONVar valueU =  myObject[keys[0]];
      JSONVar valueH =  myObject[keys[2]];
      JSONVar valueG =  myObject[keys[3]];
      JSONVar valueF =  myObject[keys[4]];

      temperature = double(valueT);
      Serial.println(temperature);
      Uid = valueU;
      Serial.println(Uid);
      humidity = double(valueH);
      Serial.println(humidity);
      smoke = valueG;
      Serial.println(smoke);
      fire = valueF;
      Serial.println(fire);

      lcd.setCursor(0, 0);
      lcd.print("Uint ID");
      lcd.setCursor(0, 1);
      lcd.print(Uid);
      delay(3000);
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Temp : " + String(temperature) + " *C ");
      lcd.setCursor(0, 1);
      lcd.print("Hum : " + String(humidity) + " %");
      delay(5000);
      lcd.clear();
      
      lcd.setCursor(0, 0);
      lcd.print(String(smoke) );
      lcd.setCursor(0, 1);
      lcd.print(String(fire));
      delay(5000);
      lcd.clear();
  }



String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
