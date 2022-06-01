#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <Esp32MQTTClient.h>

struct My_Object {
  char ssid[25];
  char pass[25];
  char cstr[150];
};

My_Object customVarr;

static bool isHubConnect = false;
unsigned long Time_Checker = 0;

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
          memcpy(Credentials.cstr, Conec_STR, sizeof(Credentials.cstr));

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
          Serial.println(customVarr.cstr);
          memcpy(Credentials.cstr, customVarr.cstr, sizeof(Credentials.cstr));
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
  Serial.print("Conection Str : "); Serial.println(customVarr.cstr);

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
  connectToWiFi(customVarr.ssid, customVarr.pass);

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)customVarr.cstr)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
}

void loop() {

  if (Serial1.available() > 0) {                /* Check for Serial input pin "Rx" */
    char bfr[5001];
    memset(bfr, 0, 5001);
    Serial1.readBytesUntil('A', bfr, 5000);   /* readBytesUntil(Terminator, data, dataLength) */
    Serial.println(bfr);

    digitalWrite(32, 1);                      /* Blink RGB Red on data recieve from Rx */
    delay(300);
    digitalWrite(32, 0);

    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)) {
      Serial.println("Succeed");
      digitalWrite(15, 1);                      /* Blink RGB Green Led on data send through mqtt */
      delay(300);
      digitalWrite(15, 0);
      delay(300);
      Serial1.write("1");                       /* To clear SD Card a signal is sent through ESP32`s TX1 to PLC */
    } else {
      Serial.println("Failure");
      digitalWrite(33, 1);                      /* Blink RGB Blue Led on data send through mqtt */
      delay(300);
      digitalWrite(33, 0);
      delay(300);
      ESP.restart();                            /* Restrart esp32 if there is a failure in send request */
    }
  }
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

  digitalWrite(14, 1);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
