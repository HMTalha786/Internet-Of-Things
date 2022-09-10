#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// ==================================================================================================================================================================================================

String SAS_Token;
String SAS_Signature;
String Azure_URL;

// ==================================================================================================================================================================================================

struct My_Object {
  char ssid[25];
  char pass[25];
  char deid[25];
  char pkey[50];
};

// ==================================================================================================================================================================================================

My_Object customVarr;

// ==================================================================================================================================================================================================

static bool isHubConnect = false;
unsigned long Time_Checker = 0;

// ==================================================================================================================================================================================================

unsigned long timer = 60000; // 1 minute

// ==================================================================================================================================================================================================

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ==================================================================================================================================================================================================

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* ----- Make object from struct to carry credentials ----- */
        My_Object Credentials;
        char WiFi_SSID[25] = "";
        char WiFi_PASS[25] = "";
        char DEVICE_ID[25] = "";
        char PRIMARY_KEY[50] = "";

        /* ----- Extract Line / Device ID ----- */
        if ( value[0] == ':' ) {

          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);
          }

          for (int i = 1; i < value.length(); i++) {
            DEVICE_ID[i - 1] = value[i];
          }

          /* ----- Device ID is new, while keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);

          Serial.print("Device ID : ");
          Serial.println(DEVICE_ID);
          memcpy(Credentials.deid, DEVICE_ID, sizeof(Credentials.deid));

          Serial.print("Primary Key : ");
          Serial.println(customVarr.pkey);
          memcpy(Credentials.pkey, customVarr.pkey, sizeof(Credentials.pkey));

          Serial.print("WiFi_Username : ");
          Serial.println(customVarr.ssid);
          memcpy(Credentials.ssid, customVarr.ssid, sizeof(Credentials.ssid));

          Serial.print("WiFi_Password : ");
          Serial.println(customVarr.pass);
          memcpy(Credentials.pass, customVarr.pass, sizeof(Credentials.pass));

        }

        /* ----- Extract Primary Key ----- */
        else if (value[0] == '=') {

          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);
          }

          for (int i = 1; i < value.length(); i++) {
            PRIMARY_KEY[i - 1] = value[i];
          }

          /* ----- Primary Key is new, while keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);

          Serial.print("Device ID : ");
          Serial.println(customVarr.deid);
          memcpy(Credentials.deid, customVarr.deid, sizeof(Credentials.deid));

          Serial.print("Primary Key : ");
          Serial.println(PRIMARY_KEY);
          memcpy(Credentials.pkey, PRIMARY_KEY, sizeof(Credentials.pkey));

          Serial.print("WiFi_Username : ");
          Serial.println(customVarr.ssid);
          memcpy(Credentials.ssid, customVarr.ssid, sizeof(Credentials.ssid));

          Serial.print("WiFi_Password : ");
          Serial.println(customVarr.pass);
          memcpy(Credentials.pass, customVarr.pass, sizeof(Credentials.pass));

        }

        /* ----- Extract WiFi Credentials ----- */
        else {

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

          /* ----- Extract WiFi Password ----- */

          for (int i = x + 1; i < value.length(); i++) {
            WiFi_PASS[i - (x + 1)] = value[i];
          }

          /* ----- WiFi Credentials are new, while keep the other Credentials Same ----- */

          Serial.print("Device ID : ");
          Serial.println(customVarr.deid);
          memcpy(Credentials.deid, customVarr.deid, sizeof(Credentials.deid));

          Serial.print("Primary Key : ");
          Serial.println(customVarr.pkey);
          memcpy(Credentials.pkey, customVarr.pkey, sizeof(Credentials.pkey));

          Serial.print("WiFi_Username : ");
          Serial.println(WiFi_SSID);
          memcpy(Credentials.ssid, WiFi_SSID, sizeof(Credentials.ssid));

          Serial.print("WiFi_Password : ");
          Serial.println(WiFi_PASS);
          memcpy(Credentials.pass, WiFi_PASS, sizeof(Credentials.pass));

        }

        /* ----- Store WiFi Credentials to EEPROM Address 0 ----- */
        EEPROM.put(0, Credentials);
        EEPROM.commit();
      }
    }

};

// ==================================================================================================================================================================================================

const char* body = "{\"SR1\":\"23456\",\"SS1\":\"0\"}";

// ==================================================================================================================================================================================================

void setup() {

  Serial.begin(500000);
  EEPROM.begin(500);
  delay(5000);
  Serial1.begin(500000);

  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("WiFi_Username : "); Serial.println(customVarr.ssid);
  Serial.print("WiFi_Password : "); Serial.println(customVarr.pass);
  Serial.print("Device ID : "); Serial.println(customVarr.deid);
  Serial.print("Primary Key : "); Serial.println(customVarr.pkey);

  /* ----- Turn ON the bluetooth ----- */
  if (digitalRead(21) == HIGH) {
    BLEDevice::init("ProCheck WiFi Shield");
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
  Connect_WiFi_Ping_Generate_SAS_Token(customVarr.ssid, customVarr.pass);

}

// ==================================================================================================================================================================================================

void loop() {

  if (WiFi.status() == WL_CONNECTED) {                         // Check WiFi connection status

    HTTPClient http;
    http.begin(Azure_URL);                                     // Specify destination for HTTP request

    http.addHeader("Content-Type", "application/json");        // Specify content-type header
    http.addHeader("Authorization", SAS_Signature);

    Serial.print("HTTP Post Request Payload Body : ");
    Serial.println(body);

    int httpResponseCode = http.POST(body);                    // Send the actual POST request

    if (httpResponseCode > 0) {
      String response = http.getString();                      // Get the response to the request
      Serial.print("HTTP Response Code : ");
      Serial.println(httpResponseCode);                        // Print return code
      Serial.println(response);                                // Print request answer
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();                                                // Free resources
  }
  delay(10000);                                                // Send a request every 10 seconds

}

// ==================================================================================================================================================================================================

void Connect_WiFi_Ping_Generate_SAS_Token(const char * ssid, const char * pwd) {

  // Connect to WiFi and print credentials ..........................................................................................

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

  // Print Network credentials .....................................................................................................

  digitalWrite(14, 1);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Primary DNS: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("Secondary DNS: ");
  Serial.println(WiFi.dnsIP(1));

  // Ping Google for checking internet .............................................................................................

  bool success = Ping.ping("www.google.com", 3);

  if (!success) {
    Serial.println("Ping failed");
    ESP.restart();
  } else {
    Serial.println("Ping successful.");
    Serial.println();
  }

  // Fetch SAS Token and Generate SAS Signature ....................................................................................

  /* Create HTTP Client Instance */
  HTTPClient http;
  http.begin("https://procheckprodfunctions.azurewebsites.net/api/GetSasToken?code=4ji8Wv3sHFDHRaoGgcjhzZos6IaWZmwgdUW2POuVUtBXRBY5%2F0MbSw%3D%3D");
  http.addHeader("Content-Type", "application/json");

  /*Create JSON Request Body */
  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["id"] = String(customVarr.deid);
  JSON_Entry["key"] = String(customVarr.pkey);

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  String request_body = JSONmessageBuffer;
  Serial.print("Request Body for fetching SAS Token : ");
  Serial.println(request_body);

  /* Make HTTP Post Request to fetch SAS Token */
  int httpResponseCode = http.POST(request_body);

  /* Handle HTTP Response i.e. Payload which is SAS Token */
  if (httpResponseCode > 0) {
    String response = http.getString();                      // Get the response to the request
    Serial.print("HTTP Response Code : ");
    Serial.println(httpResponseCode);                        // Print return code
    //Serial.println(response);                              // Print request answer

    /* Fetched SAS Token */
    SAS_Token = String(response);
    Serial.print("SAS_Token : ");
    Serial.println(SAS_Token);

    /* Whole SAS Signature for HTTP Authorization */
    SAS_Signature = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2F" + String(customVarr.deid) + "&sig=" + SAS_Token;
    Serial.print("SAS_Signature : ");
    Serial.println(SAS_Signature);

    /*Azure IoT Hub HTTP Endpoint for making Post Request*/
    Azure_URL = "https://procheck-prod.azure-devices.net/devices/" + String(customVarr.deid) + "/messages/events?api-version=2020-03-13";
    Serial.print("Azure IoT Hub Url : ");
    Serial.println(Azure_URL);

  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
