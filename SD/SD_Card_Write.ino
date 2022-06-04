#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  Serial.begin(9600);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }

  myFile = SD.open("date.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("Hello World");
    myFile.close();
    Serial.println("done.");
  } 
  
  else { Serial.println("error opening test.txt"); }
}

void loop() {
  // empty
}
