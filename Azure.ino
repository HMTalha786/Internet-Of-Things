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

struct MyObject {
   char ssid[25];
   char pass[25];
};

MyObject customVarr;

static bool isHubConnect = false;
unsigned long Time_Checker = 0;
int x = 0;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static const char* connectionString = "";

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("=============================================================");

        /* ----- Check for ',' Comma for SSID and PASS split ----- */
        Serial.print("Entered value: "); for (int i=0; i<value.length(); i++){ Serial.print(value[i]); if(value[i]==','){x = i;} }  
        Serial.print("Comma at index: "); Serial.println(x);

        /* ----- Make object from struct to carry credentials ----- */
        MyObject Credentials;                                                                      
        char WiFi_SSID[25] = "";
        char WiFi_PASS[25] = "";

        /* ----- Extract WiFi SSID ----- */
        for (int j = 0; j < x; j++){ WiFi_SSID[j]=value[j]; }                               
        Serial.println(WiFi_SSID);

        /* ----- Extract WiFi PASS ----- */
        for (int k = x+1; k < value.length(); k++){ WiFi_PASS[k-(x+1)]=value[k]; }          
        Serial.println(WiFi_PASS); 

        /* ----- Store WiFi Credentials to EEPROM Address 0 ----- */
        memcpy(Credentials.ssid,WiFi_SSID,sizeof(Credentials.ssid));                        
        memcpy(Credentials.pass,WiFi_PASS,sizeof(Credentials.pass));          
        EEPROM.put(0, Credentials);
        EEPROM.commit();
        
        Serial.println("=============================================================");
      }
    }
};

void setup() {
  Serial.begin(500000);
  EEPROM.begin(50);
  delay(5000);
  Serial1.begin(500000);
  pinMode(14, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(21, OUTPUT);

  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.println(customVarr.ssid);
  Serial.println(customVarr.pass);

  /* ----- Turn ON the bluetooth ----- */
  if(digitalRead(21) == HIGH){
    BLEDevice::init("ESP32 WIFI");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  /* ----- Connect to WiFi through saved credentials ----- */
  connectToWiFi(customVarr.ssid, customVarr.pass);

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)){ 
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
      Serial1.readBytesUntil('A',bfr,5000);     /* readBytesUntil(Terminator, data, dataLength) */ 
      Serial.println(bfr);

      digitalWrite(32, HIGH);                   /* Blink RGB Red on data recieve from Rx */
      delay(300); 
      digitalWrite(32, LOW);                   
  
    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)){
      Serial.println("Succeed");              
      digitalWrite(15, 1);                      /* Blink RGB Green Led on data send through mqtt */ 
      delay(300); 
      digitalWrite(15, 0);
      Serial1.write(1);                         /* To clear SD Card a signal is sent through ESP32`s TX1 to PLC */  
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

void connectToWiFi(const char * ssid, const char * pwd){
  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);                 /* Blink LED while connecting to WiFi */ 
    ledState = (ledState + 1) % 2;              /* Flip LED State */ 
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 600000) { ESP.restart(); }
  }
  
  digitalWrite(14, 0); 
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());
}
