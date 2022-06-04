#include <DHT.h>;

DHT dht(33, DHT22);  // Initializa pin and dht sensor type 

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
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, ");
    
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    
    delay(10000);
}

   
