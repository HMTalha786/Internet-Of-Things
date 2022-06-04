#include <WiFi.h>
#include <DHT.h>;
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Studio-Q";
const char* pass =  "Newpassword";

String url = "https://api.datacake.co/integrations/api/b03dbfec-4e6f-42ef-a2af-aac87c2738fa/";

unsigned long Time_Checker = 0;

DHT dht(16, DHT22);  // Initializa pin and dht sensor type

float hum;           // Stores humidity value
float temp;          // Stores temperature value

void setup()
{
  Serial.begin(500000);
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED
  dht.begin();
  delay(4000);   //Delay needed before calling the WiFi.begin

  connectToWiFi(ssid, pass);
}

void loop()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["device"] = "357073299541579";
  JSON_Entry["temp"] = temp;
  JSON_Entry["hum"] = hum;
  JSON_Entry["bat"] = "52.1";

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  Serial.print(JSONmessageBuffer);

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;

    http.begin(url);  //Specify destination for HTTP request
    http.addHeader("Content-Type", "application/json");             //Specify content-type header
    
    int httpResponseCode = http.POST(JSONmessageBuffer);
    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);

    if (httpResponseCode > 0) {

      String response = http.getString();               //Get the response to the request
      Serial.println(httpResponseCode);                 //Print return code
      Serial.println(response);                         //Print request answer
      digitalWrite(15, 1);
      delay(300);
      digitalWrite(15, 0);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      ESP.restart();
    }

    http.end();  //Free resources

  } else {
    Serial.println("Error in WiFi connection");
    delay(300);
    ESP.restart();
  }

  delay(10000);
}

void connectToWiFi(const char * ssid, const char * pwd) {
  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);                 /* Blink LED while connecting to WiFi */
    ledState = (ledState + 1) % 2;              /* Flip LED State */
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 60000) {
      ESP.restart();
    }
  }

  digitalWrite(14, 0);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
