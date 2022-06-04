#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  Serial.begin(9600);

  if (!SD.begin(53)) {
    Serial.println("SD Card initialization failed!");
    while (1);
  }

  myFile = SD.open("data.txt");
  
  if (myFile) {
    while (myFile.available()) { Serial.write(myFile.read()); }
    myFile.close();
  }
     
  else { Serial.println("error opening test.txt"); }
}

void loop() {
  // nothing happens after setup
}
