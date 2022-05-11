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

float hum;                    // Stores humidity value
float temp;                   // Stores temperature value
unsigned long timer = 0;

DHT dht(16, DHT22);           // Initializa pin and dht sensor type

struct My_Object {
  char ssid[25];
  char pass[25];
  char curl[150];
};

My_Object customVarr;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* ----- Make object from struct to carry credentials ----- */
        My_Object Credentials;
        char WiFi_SSID[25] = "";
        char WiFi_PASS[25] = "";
        char Conec_STR[150] = "";

        /* ----- Extract Connection String ----- */
        if ( value[0] == '@' ) {

          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);
          }

          for (int i = 1; i < value.length(); i++) {
            Conec_STR[i - 1] = value[i];
          }

          Serial.print("Connection String : ");
          Serial.println(Conec_STR);
          memcpy(Credentials.curl, Conec_STR, sizeof(Credentials.curl));

        } else {
          int x;
          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            if ( value[i] == ',' ) {
              x = i;
            }
            Serial.print(value[i]);
          }
          Serial.println("");
          Serial.print("Comma is at index : ");
          Serial.println(x);

          /* ----- Extract WiFi SSID ----- */

          for (int i = 0; i < x; i++) {
            WiFi_SSID[i] = value[i];
          }
          Serial.print("WiFi_Username : ");
          Serial.println(WiFi_SSID);
          memcpy(Credentials.ssid, WiFi_SSID, sizeof(Credentials.ssid));

          /* ----- Extract WiFi Password ----- */

          for (int i = x + 1; i < value.length(); i++) {
            WiFi_PASS[i - (x + 1)] = value[i];
          }
          Serial.print("WiFi_Password : ");
          Serial.println(WiFi_PASS);
          memcpy(Credentials.pass, WiFi_PASS, sizeof(Credentials.pass));

          /* ----- Keep the Connection String Same ----- */

          EEPROM.get(0, customVarr);
          Serial.println(customVarr.curl);
          memcpy(Credentials.curl, customVarr.curl, sizeof(Credentials.curl));
        }

        /* ----- Store WiFi Credentials to EEPROM Address 0 ----- */
        EEPROM.put(0, Credentials);
        EEPROM.commit();
      }
    }

};

void setup() {
  Serial.begin(500000);
  EEPROM.begin(500);
  delay(5000);
  dht.begin();
  pinMode(16, INPUT);   // DHT Sensor
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("WiFi_Username : "); Serial.println(customVarr.ssid);
  Serial.print("WiFi_Password : "); Serial.println(customVarr.pass);
  Serial.print("Conection URL : "); Serial.println(customVarr.curl);

  /* ----- Turn ON the bluetooth ----- */
  if (digitalRead(21) == HIGH) {
    BLEDevice::init("ESP32 Weather Device");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  /* ----- Connect to WiFi through saved credentials ----- */
  connectToWiFi(customVarr.ssid, customVarr.pass);
}

void loop() {
  http_request();
  delay(10000);
}

void http_request() {

  if (WiFi.status() == WL_CONNECTED) {                              //Check WiFi connection status

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
    Serial.println(JSONmessageBuffer);
    HTTPClient http;
    http.begin(customVarr.curl);                                    //Specify destination for HTTP request
    http.addHeader("Content-Type", "application/json");             //Specify content-type header

    int httpResponseCode = http.POST(JSONmessageBuffer);
    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);

    if (httpResponseCode == 200) {
      String response = http.getString();                                                    //Get the response to the request
      Serial.print("HTTP Response Code : "); Serial.println(httpResponseCode);               //Print return code
      Serial.print("Response Payload : "); Serial.println(response);                         //Print request answer
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
    http.end();     //Free resources

  } else {
    Serial.println("Error in WiFi connection");
    delay(300);
    ESP.restart();
  }
}

void connectToWiFi(const char * ssid, const char * pwd) {
  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  timer = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);                 /* Blink LED while connecting to WiFi */
    ledState = (ledState + 1) % 2;              /* Flip LED State */
    delay(500);
    Serial.print(".");
    if (millis() - timer > 60000) {
      ESP.restart();
    }
  }
  digitalWrite(14, 0);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
