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

// Buffer Array
String val[150];

// Array Index
int arr_index = 0;

// Array Read Status
bool arr_flag = false;

// JSON Packet Sending time Counter
unsigned long timer = 10000;        // 10 Seconds

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;
int SS5 = 0;
int SS6 = 0;

// Sensor 1, 3, 5 Previous Status Bit
int PS1 = 0;
int PS3 = 0;
int PS5 = 0;

// Sensor`s Input Pin Values
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;
int S5 = 0;
int S6 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;
int P5 = 0;
int P6 = 0;

// Sensor`s Count Values
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;
unsigned long count5 = 0;
unsigned long count6 = 0;

// Sensor`s Down Time Stamps for continuous low input
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;
unsigned long DTS3 = 0;
unsigned long DTS4 = 0;
unsigned long DTS5 = 0;
unsigned long DTS6 = 0;

// Sensor`s Continuous Time Stamps for continuous high input
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;
unsigned long CTS3 = 0;
unsigned long CTS4 = 0;
unsigned long CTS5 = 0;
unsigned long CTS6 = 0;

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

  /* ----- Connect to WiFi and Fetch SAS Token through saved credentials ----- */
  Connect_WiFi_Ping_Generate_SAS_Token(customVarr.ssid, customVarr.pass);

}

// ==================================================================================================================================================================================================

void loop() {

  if ( WiFi.status() == WL_CONNECTED && millis() >= timer ) {             // Check WiFi connection status and timer

    StaticJsonBuffer<300> JSON_Packet;
    JsonObject& JSON_Entry = JSON_Packet.createObject();

    JSON_Entry["PTS"] = millis();
    JSON_Entry["SR1"] = count1;
    JSON_Entry["SR2"] = count2;
    JSON_Entry["SR3"] = count3;
    JSON_Entry["SR4"] = count4;
    JSON_Entry["SR5"] = count5;
    JSON_Entry["SR6"] = count6;
    JSON_Entry["SS1"] = SS1;
    JSON_Entry["SS2"] = SS2;
    JSON_Entry["SS3"] = SS3;
    JSON_Entry["SS4"] = SS4;
    JSON_Entry["SS5"] = SS5;
    JSON_Entry["SS6"] = SS6;

    char JSONmessageBuffer[300];
    JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    String Request_Body = JSONmessageBuffer;

    HTTPClient http;
    http.begin(Azure_URL);                                     // Specify destination for HTTP request

    http.addHeader("Content-Type", "application/json");        // Specify content-type header
    http.addHeader("Authorization", SAS_Signature);

    Serial.print("HTTP Post Request Body : ");
    Serial.println(Request_Body);

    digitalWrite(32, 1);                                       /* Blink RGB Red on data recieve from Rx */
    delay(300);
    digitalWrite(32, 0);

    int httpResponseCode = http.POST(Request_Body);            // Send the actual POST request

    if (httpResponseCode > 0) {
      String response = http.getString();                      // Get the response to the request
      Serial.print("HTTP Response Code : ");
      Serial.println(httpResponseCode);                        // Print return code
      Serial.println(response);                                // Print request answer
      Serial.println("Successfully Send Data Packet");
      digitalWrite(15, 1);                                     /* Blink RGB Green Led on successfully data send through mqtt */
      delay(300);
      digitalWrite(15, 0);
      //Serial1.write("4");                                    /* ----- Data successfully sent signal to PLC ----- */
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      Serial.println("Failed to Send Data Packet");
      digitalWrite(33, 1);                                     /* Blink RGB Blue Led on failure in data sending through mqtt */
      delay(300);
      digitalWrite(33, 0);
      delay(300);
      WiFi.disconnect();                                       /* Restrart esp32 if there is a failure in send request */
      Connect_WiFi_Ping_Generate_SAS_Token(customVarr.ssid, customVarr.pass);
    }
    http.end();                                                // Free resources
    timer = millis() + 60000UL;
  }
  
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
