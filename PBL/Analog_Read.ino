int inMin = 0;
int inMax = 1023;
int outMin = 0;
int outMax = 1300;

String a;
String v;
String c;
String f;
String json;

//  float flow = ( input ) / ( inMax * outMax );

void setup() {
  Serial.begin(500000UL);
}

void loop() {

  int input = analogRead(I0_12);

  float volt = map(input, 0, 1023, 0, 10000);
  float curr = map(input, 0, 1023, 0, 20000);
  float flow = map(input, 0, 1023, 0, 1300);

  a = "\"Analog Resolution\":" + String(input) + " bits" + "," ;
  
  v = "\"Voltages\":" + String(volt/1000) + " volts" + "," ;
  
  c = "\"Current\":" + String(curr/1000) + " mA" + "," ;
  
  f = "\"Flow Value\":" + String(flow) + " kg/h" ;

  json = "{" + a + v + c + f + "}" ;
  Serial.println(json);
  Serial.println("");

  delay(1000);
}
