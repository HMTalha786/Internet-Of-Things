#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  Serial.begin(9600);
  SD.begin(53);
  
  if (!SD.begin(53)) {
    Serial.println("SD Card initialization failed!");
    while (1);
  }
}

void loop() {

  //for the Rx response 1 or 0
  // delay(300); 

  if(Serial.available() > 0){ // Rx = 1 means Blue led, write json to sd card
    
    // Write data to File
    myFile = SD.open("date.txt", FILE_WRITE); 

    if (myFile) {
    myFile.println("JSON");
    myFile.close();
    }   
  }
  
  else { // Rx=0 means green led, read json from sd card
    
    // Read data from File
    myFile = SD.open("data.txt");
  
    if (myFile) {
      while (myFile.available()) { Serial.write(myFile.read()); }
      myFile.close();

      // Deleting File
      SD.remove("data.txt");
    }
  }
}
