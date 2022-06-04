void setup(){
 Serial.begin(9600UL);
}

void loop(){
  
  int input = analogRead(I0_12);
  float voltage = map(input, 0, 1023, 0, 10000);
  float current = map(input, 0, 1023, 0, 20000);
  float transmitter = map(input, 0, 1023, 0, 20000);

  Serial.print("\t Current = ");  Serial.print(current/1000);   Serial.print("mA");
  Serial.print("\t Voltage = ");  Serial.print(voltage/1000);   Serial.println("V");
  Serial.print("\t Resistance = ");  Serial.print(transmitter/1000);   Serial.println("Ohm");

  delay(1000);
}
