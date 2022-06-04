#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Studio-Q"
#define WIFI_PASSWORD "Newpassword"
#define API_KEY "AIzaSyDc7JHnqc_rUmU-UsCG1pwh5YzuJNFg6Do"
#define DATABASE_URL "https://is-esp32-plc-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){ Serial.print("."); delay(300); }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY; 

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL; 

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){ Serial.println("ok"); signupOK = true; }
  else{ Serial.printf("%s\n", config.signer.signupError.message.c_str()); }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)){
    
    sendDataPrevMillis = millis();
    
    // ......................... Write an Int number on the database path test/int ................................
    
    if (Firebase.RTDB.setInt(&fbdo, "test/Count", count)){
      Serial.println("PASSED INT");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED INT");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // ......................... Write an Float number on the database path test/float .............................
    
    if (Firebase.RTDB.setFloat(&fbdo, "test/Status", 0.01 + random(0,100))){
      Serial.println("PASSED FLOAT");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED FLOAT");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
