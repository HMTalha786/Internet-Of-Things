#include <RTC.h>

const int YEAR = 2018;
const int MONTH = 3;
const int DAY = 28;
const int HOUR = 13;
const int MINUTE = 24;
const int SECOND = 25;

void setup() {
  Serial.begin(9600L);
  Serial.println("Configuring RTC: ");
  Serial.println();
  
  RTC.setYear(YEAR);                      //sets year
  RTC.setMonth(MONTH);                    //sets month
  RTC.setMonthDay(DAY);                   //sets day
  RTC.setHour(HOUR);                      //sets hour
  RTC.setMinute(MINUTE);                  //sets minute
  RTC.setSecond(SECOND);                  //sets second

  //RTC.write writes in the RTC memory all that has been set
  if(!RTC.write()){                       
    Serial.println("RTC Write Error: Are the switches well placed ???");
  }
}

void loop() {
  if (!RTC.read()) {
    Serial.println("RTC Read Error: is time set ???");
  } else {
    Serial.print("Time: ");
    Serial.print(RTC.getYear());
    Serial.print("-");
    Serial.print(RTC.getMonth());
    Serial.print("-");
    Serial.print(RTC.getMonthDay());
    Serial.print(" ");
    Serial.print(RTC.getHour());
    Serial.print(":");
    Serial.print(RTC.getMinute());
    Serial.print(":");
    Serial.print(RTC.getSecond());
    Serial.print(" (");
    Serial.print(RTC.getTime());
    Serial.println(")");
 }
 delay(1000);
}
