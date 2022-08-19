#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Esp32MQTTClient.h>

// JSON Packet Sending time Counter ( After every 10 sec )
unsigned long previous_time = 10000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS0;
int SS1;
int SS2;
int SS3;
int SS4;
int SS5;
int SS6;
int SS7;
int SS8;
int SS9;

// Sensor`s Input Pin Values
int S0 = 0;
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;
int S5 = 0;
int S6 = 0;
int S7 = 0;
int S8 = 0;
int S9 = 0;

// Sensor`s Input Pin Previous Values
int P0 = 0;
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;
int P5 = 0;
int P6 = 0;
int P7 = 0;
int P8 = 0;
int P9 = 0;

// Sensor`s Count Values
unsigned long count0 = 0;
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;
unsigned long count5 = 0;
unsigned long count6 = 0;
unsigned long count7 = 0;
unsigned long count8 = 0;
unsigned long count9 = 0;

// Sensor`s Down Time Stamps for continuous low input
unsigned long DTS0 = 0;
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;
unsigned long DTS3 = 0;
unsigned long DTS4 = 0;
unsigned long DTS5 = 0;
unsigned long DTS6 = 0;
unsigned long DTS7 = 0;
unsigned long DTS8 = 0;
unsigned long DTS9 = 0;

// Sensor`s Continuous Time Stamps for continuous high input
unsigned long CTS0 = 0;
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;
unsigned long CTS3 = 0;
unsigned long CTS4 = 0;
unsigned long CTS5 = 0;
unsigned long CTS6 = 0;
unsigned long CTS7 = 0;
unsigned long CTS8 = 0;
unsigned long CTS9 = 0;

// WiFi and Azure Credentials
const char* ssid     = "Studio Phi";
const char* pass     = "StudioPhi124";
static const char* connectionString = "---------";
static bool hasIoTHub = false;
unsigned long Time_Checker = 0;

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  Serial.begin(500000);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);
}

//Task1code: Sensor pins inputs
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  while (true) {
    // Sensor 0 ..............................................................................
    S0 = digitalRead(I0_0);
    if ( S0 == 1 && P0 == 0 ) { count0++; SS0=0; CTS0 = millis(); }              
    if ( S0 == 0 && P0 == 1 ) { DTS0 = millis(); CTS0 = 0; }                      
    if ( S0 == 0 && P0 == 0 && ( ( millis() - DTS0 ) > DT ) ) { SS0=1; }         
    if ( S0 == 1 && P0 == 1 && ( ( millis() - CTS0 ) > DT ) ) { SS0=1; }         
    P0 = S0;
  
    // Sensor 1 ..............................................................................
    S1 = digitalRead(I0_1);
    if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
    if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }                      
    if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         
    if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         
    P1 = S1;
  
    // Sensor 2 ..............................................................................
    S2 = digitalRead(I0_2);
    if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
    if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
    if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
    if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
    P2 = S2;
  
    // Sensor 3 ..............................................................................
    S3 = digitalRead(I0_3);
    if ( S3 == 1 && P3 == 0 ) { count3++; SS3=0; CTS3 = millis(); } 
    if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; }
    if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
    if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
    P3 = S3;
  
    // Sensor 4 ..............................................................................
    S4 = digitalRead(I0_4);
    if ( S4 == 1 && P4 == 0 ) { count4++; SS4=0; CTS4 = millis(); } 
    if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; }
    if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > DT ) ) { SS4=1; } 
    if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > DT ) ) { SS4=1; } 
    P4 = S4;

    // Sensor 5 ..............................................................................
    S5 = digitalRead(I0_5);
    if ( S5 == 1 && P5 == 0 ) { count5++; SS5=0; CTS5 = millis(); } 
    if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; }
    if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > DT ) ) { SS5=1; } 
    if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > DT ) ) { SS5=1; } 
    P5 = S5;
  
    // Sensor 6 ..............................................................................
    S6 = digitalRead(I0_6);
    if ( S6 == 1 && P6 == 0 ) { count6++; SS6=0; CTS6 = millis(); } 
    if ( S6 == 0 && P6 == 1 ) { DTS6 = millis(); CTS6 = 0; }
    if ( S6 == 0 && P6 == 0 && ( ( millis() - DTS6 ) > DT ) ) { SS6=1; } 
    if ( S6 == 1 && P6 == 1 && ( ( millis() - CTS6 ) > DT ) ) { SS6=1; } 
    P6 = S6;
  
    // Sensor 7 ..............................................................................
    S7 = digitalRead(I0_7);
    if ( S7 == 1 && P7 == 0 ) { count7++; SS7=0; CTS7 = millis(); }              
    if ( S7 == 0 && P7 == 1 ) { DTS7 = millis(); CTS7 = 0; }                      
    if ( S7 == 0 && P7 == 0 && ( ( millis() - DTS7 ) > DT ) ) { SS7=1; }         
    if ( S7 == 1 && P7 == 1 && ( ( millis() - CTS7 ) > DT ) ) { SS7=1; }         
    P7 = S7;
  
    // Sensor 8 ..............................................................................
    S8 = digitalRead(I0_8);
    if ( S8 == 1 && P8 == 0 ) { count8++; SS8=0; CTS8 = millis(); }              
    if ( S8 == 0 && P8 == 1 ) { DTS8 = millis(); CTS8 = 0; }                      
    if ( S8 == 0 && P8 == 0 && ( ( millis() - DTS8 ) > DT ) ) { SS8=1; }         
    if ( S8 == 1 && P8 == 1 && ( ( millis() - CTS8 ) > DT ) ) { SS8=1; }         
    P8 = S8;

    // Sensor 9 ..............................................................................
    S9 = digitalRead(I0_9);
    if ( S9 == 1 && P9 == 0 ) { count9++; SS9=0; CTS9 = millis(); }              
    if ( S9 == 0 && P9 == 1 ) { DTS9 = millis(); CTS9 = 0; }                      
    if ( S9 == 0 && P9 == 0 && ( ( millis() - DTS9 ) > DT ) ) { SS9=1; }         
    if ( S9 == 1 && P9 == 1 && ( ( millis() - CTS9 ) > DT ) ) { SS9=1; }         
    P9 = S9;
  }
}

// Task2code: Send to azure
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  while (true) {
    connect_to_wifi(ssid, pass);
    delay(100);
    send_to_azure();
    delay(200000);
    WiFi.disconnect();
  }
}

void connect_to_wifi(const char * ssid, const char * pwd) {
  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);                 /* Blink 33 while connecting to WiFi */
    ledState = (ledState + 1) % 2;              /* Flip 33 State */
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 60000) {
      WiFi.disconnect();
      connect_to_wifi(ssid, pass);
    }
  }
  digitalWrite(14, 1);
  Serial.println();
  Serial.println("WiFi connected!");

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)) {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  Serial.println("Start sending JSON Packet to Azure.");
}

void send_to_azure() {
  
  if (hasIoTHub) {
    
    StaticJsonBuffer<900> JSON_Packet;   
    JsonObject& JSON_Entry = JSON_Packet.createObject();
  
    // Declaring JSON Key Value Pairs of Sensor Counts ...................................................
    JSON_Entry["SR0"] = count0;
    JSON_Entry["SR1"] = count1;
    JSON_Entry["SR2"] = count2;
    JSON_Entry["SR3"] = count3;
    JSON_Entry["SR4"] = count4;
    JSON_Entry["SR5"] = count5;
    JSON_Entry["SR6"] = count6;
    JSON_Entry["SR7"] = count7;
    JSON_Entry["SR8"] = count8;
    JSON_Entry["SR9"] = count9;
  
    // Declaring JSON Key Value Pairs of Sensor Status ...................................................
    JSON_Entry["SS0"] = SS0;
    JSON_Entry["SS1"] = SS1;
    JSON_Entry["SS2"] = SS2;
    JSON_Entry["SS3"] = SS3;
    JSON_Entry["SS4"] = SS4;
    JSON_Entry["SS5"] = SS5;
    JSON_Entry["SS6"] = SS6;
    JSON_Entry["SS7"] = SS7;
    JSON_Entry["SS8"] = SS8;
    JSON_Entry["SS9"] = SS9;
  
    char JSONmessageBuffer[900];
    JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    
    char buff[1500]; 
    snprintf(buff, 1500, JSONmessageBuffer);
    
    if (Esp32MQTTClient_SendEvent(buff)) {
      Serial.println("Succeed ...");
    }
    else {
      Serial.println("Failure ...");
      WiFi.disconnect();
      connect_to_wifi(ssid, pass);
    }
  }
}

void loop() {}
