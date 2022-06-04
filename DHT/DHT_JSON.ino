#include <DHT.h>;
#include <ArduinoJson.h>

DHT dht(21, DHT22);  // Initializa pin and dht sensor type 

float hum;           // Stores humidity value
float temp;          // Stores temperature value

void setup()
{
  Serial.begin(500000);
  dht.begin();
}

void loop()
{
    hum = dht.readHumidity();
    temp= dht.readTemperature();

    StaticJsonBuffer<300> JSON_Packet;  
    JsonObject& JSON_Entry = JSON_Packet.createObject();

    JSON_Entry["device"] = "357073299541579";
    JSON_Entry["temp"] = temp;
    JSON_Entry["hum"] = hum;

    char JSONmessageBuffer[300];
    JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

    Serial.print(JSONmessageBuffer);
    
    delay(10000);
}

   
