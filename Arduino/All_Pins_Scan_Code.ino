#include <ArduinoJson.h>

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
int SS10;
int SS11;
int SS12;

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
int S10 = 0;
int S11 = 0;
int S12 = 0;

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
int P10 = 0;
int P11 = 0;
int P12 = 0;

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
unsigned long count10 = 0;
unsigned long count11 = 0;
unsigned long count12 = 0;

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
unsigned long DTS10 = 0;
unsigned long DTS11 = 0;
unsigned long DTS12 = 0;

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
unsigned long CTS10 = 0;
unsigned long CTS11 = 0;
unsigned long CTS12 = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {

  // JSON Packet Sent after every 10 sec ...................................................          
  if ( millis() >= previous_time ) { previous_time = millis()+10000UL; json_packet_sender(); }

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

  // Sensor 10 ..............................................................................
  S10 = digitalRead(I0_10);
  if ( S10 == 1 && P10 == 0 ) { count10++; SS10=0; CTS10 = millis(); }              
  if ( S10 == 0 && P10 == 1 ) { DTS10 = millis(); CTS10 = 0; }                      
  if ( S10 == 0 && P10 == 0 && ( ( millis() - DTS10 ) > DT ) ) { SS10=1; }         
  if ( S10 == 1 && P10 == 1 && ( ( millis() - CTS10 ) > DT ) ) { SS10=1; }         
  P10 = S10;

  // Sensor 11 ..............................................................................
  S11 = digitalRead(I0_11);
  if ( S11 == 1 && P11 == 0 ) { count11++; SS11=0; CTS11 = millis(); }              
  if ( S11 == 0 && P11 == 1 ) { DTS11 = millis(); CTS11 = 0; }                      
  if ( S11 == 0 && P11 == 0 && ( ( millis() - DTS11 ) > DT ) ) { SS11=1; }         
  if ( S11 == 1 && P11 == 1 && ( ( millis() - CTS11 ) > DT ) ) { SS11=1; }         
  P11 = S11;

  // Sensor 12 ..............................................................................
  S12 = digitalRead(I0_12);
  if ( S12 == 1 && P12 == 0 ) { count12++; SS12=0; CTS12 = millis(); }              
  if ( S12 == 0 && P12 == 1 ) { DTS12 = millis(); CTS12 = 0; }                      
  if ( S12 == 0 && P12 == 0 && ( ( millis() - DTS12 ) > DT ) ) { SS12=1; }         
  if ( S12 == 1 && P12 == 1 && ( ( millis() - CTS12 ) > DT ) ) { SS12=1; }         
  P12 = S12;
}

void json_packet_sender(){
  
  // Declaring static JSON buffer ......................................................................
  StaticJsonBuffer<900> JSON_Packet;   

  // Creating JSON Object ..............................................................................
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
  JSON_Entry["SR10"] = count10;
  JSON_Entry["SR11"] = count11;
  JSON_Entry["SR12"] = count12;

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
  JSON_Entry["SS10"] = SS10;
  JSON_Entry["SS11"] = SS11;
  JSON_Entry["SS12"] = SS12;

  char JSONmessageBuffer[900];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  Serial.print(JSONmessageBuffer);
  Serial.println('A');

  digitalWrite(Q0_4, HIGH);
  delay(10);
  digitalWrite(Q0_4, LOW);
}
