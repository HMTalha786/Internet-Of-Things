#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  Serial.begin(9600);
  
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }

  Serial.println("Removing example.txt...");
  SD.remove("json.txt");
  SD.remove("data.txt");
}

void loop() {
  // nothing happens after setup finishes.
}
