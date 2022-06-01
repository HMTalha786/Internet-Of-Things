/* Enables bluetooth when pin 21 is high .................... Done
   Get data from BLE App .................................... Done
   Parse it into respective credential variables ............ Done
   Store the data into EEPROM ............................... Done
   Connect to WiFi through saved values ..................... Done
   Establish Connection with Azure IoT Hub .................. Done
   Read data from Serial Input .............................. Done
   Send data to cloud using MQTT ............................ Done
   Blink LEDs on the basis of events ........................ Done
*/

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

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int x;
static const char* connectionString = "HostName=procheck-prod.azure-devices.net;DeviceId=test1_vuqxi;SharedAccessKey=mtxT99ISbTIEQ34BRlTLGnvF8U1SPFvHxZssgaYx8oI=";
static bool isHubConnect = false;
unsigned long Time_Checker = 0;

struct MyObject {
  char ssid[25];
  char pass[25];
};

MyObject WiFi_Credentials;

/*=====================================================================================================================================================================*/

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* Create new variable from struct to store credentials */
        MyObject cred; 
        char WiFi_SSID[25]; 
        char WiFi_PASS[25];

        /* Check for the index of ',' */
        for (int i = 0; i < value.length(); i++) {
          if (value[i] == ',') {
            x = i;
          }
        }
        /* Parse WiFi SSID from bluetooth input value */
        for (int j = 0; j < x; j++) {
          WiFi_SSID[j] = value[j];
        }
        /* Parse WiFi PASS from bluetooth input value */
        for (int k = x + 1; k < value.length(); k++) {
          WiFi_PASS[k - (x + 1)] = value[k];
        }

        Serial.print("WiFi_SSID : "); Serial.println(WiFi_SSID);                             /* Print WiFi_SSID on Serial Monitor */
        Serial.print("WiFi_PASS : "); Serial.println(WiFi_PASS);                             /* Print WiFi_PASS on Serial Monitor */

        memcpy(cred.ssid, WiFi_SSID, sizeof(cred.ssid));                                     /* Store WiFi SSID in Struct SSID Variable*/
        memcpy(cred.pass, WiFi_PASS, sizeof(cred.pass));                                     /* Store WiFi PASS in Struct PASS Variable*/
        EEPROM.put(0, cred);                                                                 /* Write WiFi SSID and PASS to EEPROM */
        EEPROM.commit();                                                                     /* Safely enclose the EEPROM */
      }
    }
};

/*=====================================================================================================================================================================*/

void setup() {
  Serial.begin(500000);
  Serial1.begin(500000);
  EEPROM.begin(50);
  pinMode(21, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);

  if (digitalRead(21) == HIGH) {
    BLEDevice::init("PROCHECK WIFI SHIELD");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  EEPROM.get(0, WiFi_Credentials);

  Serial.print("WiFi_SSID : ");           Serial.println(WiFi_Credentials.ssid);
  Serial.print("WiFi_PASS : ");           Serial.println(WiFi_Credentials.pass);
  Serial.print("Connection String : ");   Serial.println(connectionString);

  Serial.println("Connecting to WiFi...");
  connectToWiFi(WiFi_Credentials.ssid, WiFi_Credentials.pass);

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    delay(300);
    ESP.restart();
    return;
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
}

/*=====================================================================================================================================================================*/

void loop() {
  if (Serial1.available() > 0) {
    char bfr[5001];
    memset(bfr, 0, 5001);
    Serial1.readBytesUntil('A', bfr, 5000);   // Serial1.readBytesUntil('>', data, dataLength);
    digitalWrite(32, HIGH);                   // Blink Red on data recieve from Rx
    delay(300);
    digitalWrite(32, LOW);
    Serial.println(bfr);

    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)) {
      Serial.println("Succeed");
      digitalWrite(15, 1);                     // Blink Green Led on data send through mqtt
      delay(300);
      digitalWrite(15, 0);
      Serial1.write("1");                      // To clear SD Card
    } else {
      Serial.println("Failure");
      digitalWrite(33, 1);                     // Blink Blue Led on data send through mqtt
      delay(300);
      digitalWrite(33, 0);
      ESP.restart();
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
