#include <ModbusMaster.h>

#define MAX485_DE 14    // Driver Enable
#define MAX485_RE 32    // Receiver Enable

ModbusMaster node;

// Duration Handling ================================================================================================================================================================

unsigned long Pre_Millis = 0;
unsigned long Cur_Millis = millis();
unsigned long Duration = 1000;

// EA Data Registers  ===============================================================================================================================================================

#define VDR_L1 0x0280           // L1 Voltage Data Register
#define ADR_L1 0x0282           // L1 Current Data Register
#define TPDR_L1 0x0284          // L1 True Power Data Register
#define APDR_L1 0x02A0          // L1 Apparent Power Data Register

#define VDR_L2 0x0286           // L2 Voltage Data Register
#define ADR_L2 0x0288           // L2 Current Data Register
#define TPDR_L2 0x028A          // L2 True Power Data Register
#define APDR_L2 0x02A2          // L2 Apparent Power Data Register

#define VDR_L3 0x028C           // L3 Voltage Data Register
#define ADR_L3 0x028E           // L3 Current Data Register
#define TPDR_L3 0x0290          // L3 True Power Data Register
#define APDR_L3 0x02A4          // L3 Apparent Power Data Register

#define TTPDR 0x029E            // Total True Power Data Register
#define TAPDR 0x02A6            // Total Apparent Power Data Register

#define PVDR_L12 0x0292         // L1 - L2 Phase Voltage Data Register
#define PVDR_L23 0x0294         // L2 - L3 Phase Voltage Data Register
#define PVDR_L31 0x0296         // L3 - L1 Phase Voltage Data Register

#define TFDR 0x02B8             // Total Frequency Data Register

// L1 Phase Parameters ==============================================================================================================================================================

float V_L1;     // L1 Voltage
float A_L1;     // L1 Current
float TP_L1;    // L1 True Power
float AP_L1;    // L1 Apparent Power
float RP_L1;    // L1 Reactive Power
float PF_L1;    // L1 Power Factor

// L2 Phase Parameters ==============================================================================================================================================================

float V_L2;     // L2 Voltage
float A_L2;     // L2 Current
float TP_L2;    // L2 True Power
float AP_L2;    // L2 Apparent Power
float RP_L2;    // L2 Reactive Power
float PF_L2;    // L2 Power Factor

// L3 Phase Parameters ===============================================================================================================================================================

float V_L3;     // L3 Voltage
float A_L3;     // L3 Current
float TP_L3;    // L3 True Power
float AP_L3;    // L3 Apparent Power
float RP_L3;    // L3 Reactive Power
float PF_L3;    // L3 Power Factor

// 3 Phase Aggregated Parameters ===================================================================================================================================================================

float TTP;      // Total True Power
float TAP;      // Total Apparent Power
float TRP;      // Total Reactive Power
float TPF;      // Total Power Factor
float TF;       // Total Frequency

// Line to Line Voltages ===================================================================================================================================================================

float LV_12;    // L1-L2 Voltages
float LV_23;    // L2-L3 Voltages
float LV_31;    // L3-L1 Voltages

// =======================================================================================================================================================================================

void preTransmission()
{
  digitalWrite(MAX485_RE, 1);
  digitalWrite(MAX485_DE, 1);
}

// =======================================================================================================================================================================================

void postTransmission()
{
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);
}

// =======================================================================================================================================================================================

float getData( uint16_t dataRegister, float correction_factor ) {
  float result;
  uint8_t dataRegisterNode = node.readHoldingRegisters(dataRegister, 2);
  if ( dataRegisterNode == node.ku8MBSuccess )                       // [readHoldingRegisters(Address of the register, Number of registers want to be read from this address)]
  {
    uint16_t data_LSB = node.getResponseBuffer(0x00);
    uint16_t data_MSB = node.getResponseBuffer(0x01);

    uint16_t Data[2] = {data_LSB, data_MSB};
    uint16_t joinedData;
    memcpy(&joinedData, Data, 4);
    result = joinedData * correction_factor;
  }
  return result;

}

// =======================================================================================================================================================================================

void setup() {

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);

  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);

  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 17);  // ( Baud rate = 9600, Configuration = [8 Data bits,No parity,1 Stop bit], Rx pin, Tx pin)

  node.begin(1, Serial1);                   // Modbus slave ID 1

  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

}

// =======================================================================================================================================================================================

void loop() {

  if ( ( Cur_Millis - Pre_Millis ) >= Duration ) {

    Pre_Millis = Cur_Millis;

    Serial.println(" ________________________________________________________ Line 1 Parameters ________________________________________________________ ");

    Serial.print("L1 Voltage : ");
    V_L1 = getData(VDR_L1, 0.1);
    Serial.println(V_L1);
    delay(10);

    Serial.print("L1 Current : ");
    A_L1 = getData(ADR_L1, 0.007);
    Serial.println(A_L1);
    delay(10);

    Serial.print("L1 True Power : ");
    TP_L1 = getData(TPDR_L1, 0.7);
    Serial.println(TP_L1);
    delay(10);

    Serial.print("L1 Apparent Power : ");
    AP_L1 = getData(APDR_L1, 0.7);
    Serial.println(AP_L1);
    delay(10);

    Serial.print("L1 Reactive Power : ");
    RP_L1 = sqrt( sq(AP_L1) - sq(TP_L1) );
    Serial.println(RP_L1);
    delay(10);

    Serial.print("L1 Power Factor : ");
    if (AP_L1 > 0) { PF_L1 = TP_L1 / AP_L1; } else { PF_L1 = 0.0; }
    Serial.println(PF_L1);
    delay(10);

    Serial.println(" ________________________________________________________ Line 2 Parameters ________________________________________________________ ");
  
    Serial.print("L2 Voltage : ");
    V_L2 = getData(VDR_L2, 0.1);
    Serial.println(V_L2);
    delay(10);
  
    Serial.print("L2 Current : ");
    A_L2 = getData(ADR_L2, 0.007);
    Serial.println(A_L2);
    delay(10);
  
    Serial.print("L2 True Power : ");
    TP_L2 = getData(TPDR_L2, 0.7);
    Serial.println(TP_L2);
    delay(10);
  
    Serial.print("L2 Apparent Power : ");
    AP_L2 = getData(APDR_L2, 0.7);
    Serial.println(AP_L2);
    delay(10);
    
    Serial.print("L2 Reactive Power : ");
    RP_L2 = sqrt(sq(AP_L2) - sq(TP_L2));
    Serial.println(RP_L2);
    delay(10);
    
    Serial.print("L2 Power Factor : ");
    if (AP_L2 > 0) { PF_L2 = TP_L2 / AP_L2; } else { PF_L2 = 0.0; }
    Serial.println(PF_L2);
    delay(10);
  
    Serial.println(" ________________________________________________________ Line 3 Parameters ________________________________________________________ ");
  
    Serial.print("L3 Voltage : ");
    V_L3 = getData(VDR_L3, 0.1);
    Serial.println(V_L3);
    delay(10);
  
    Serial.print("L3 Current : ");
    A_L3 = getData(ADR_L3, 0.007);
    Serial.println(A_L3);
    delay(10);
  
    Serial.print("L3 True Power : ");
    TP_L3 = getData(TPDR_L3, 0.7);
    Serial.println(TP_L3);
    delay(10);
  
    Serial.print("L3 Apparent Power : ");
    AP_L3 = getData(APDR_L3, 0.7);
    Serial.println(AP_L3);
    delay(10);
    
    Serial.print("L3 Reactive Power : ");
    RP_L3 = sqrt(sq(AP_L3) - sq(TP_L3));
    Serial.println(RP_L3);
    delay(10);
    
    Serial.print("L3 Power Factor : ");
    if (AP_L3 > 0) { PF_L3 = TP_L3 / AP_L3; } else { PF_L1 = 0.0; }
    Serial.println(PF_L3);
    delay(10);
   
    Serial.println(" ________________________________________________________ Total 3 Phase Aggregated Values ________________________________________________________ ");
  
    Serial.print("Total Frequency ");
    TF = getData(TFDR, 0.1);
    Serial.println(TF);
    delay(10);
    
    Serial.print("Total 3 Phase True Power ");
    TTP = getData(TTPDR, 0.7);
    Serial.println(TTP);
    delay(10);
  
    Serial.print("Total 3 Phase Apparent Power ");
    TAP = getData(TAPDR, 0.7);
    Serial.println(TAP);
    delay(100);
  
    Serial.print("Total 3 Phase Reactive Power ");
    TRP = sqrt(sq(TAP) - sq(TTP));
    Serial.println(TRP);
    delay(100);
  
    Serial.print("Total 3 Phase Power Factor ");
    if (TAP > 0) { TPF = TTP / TAP; } else { TPF = 0.0; }
    Serial.println(TPF);
    delay(10);
  
    Serial.println(" ________________________________________________________ Line to Line / Phase Voltages  ________________________________________________________ ");
    
    Serial.print("L1-L2 Phase Voltage ");
    LV_12 = getData(PVDR_L12, 0.1);
    Serial.println(LV_12);
    delay(10);
  
    Serial.print("L2-L3 Phase Voltage ");
    LV_23 = getData(PVDR_L23, 0.1);
    Serial.println(LV_23);
    delay(10);
  
    Serial.print("L3-L1 Phase Voltage ");
    LV_31 = getData(PVDR_L31, 0.1);
    Serial.println(LV_31);
    delay(10);

  }
}
