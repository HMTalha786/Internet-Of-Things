#include <WiFi.h>
#include <Esp32MQTTClient.h>

static bool isHubConnect = false;
unsigned long Time_Checker = 0;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const char* ssid     = "HomeNet-102-0311-2051551";
const char* pass     = "Newpassword";
static const char* connectionString = "HostName=<hostname>;DeviceId=<LineId>;SharedAccessKey=<PrimaryKey>";

void setup() {
  Serial.begin(500000);
  delay(5000);
  Serial1.begin(500000);
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  /* ----- Connect to WiFi through saved credentials ----- */
  connectToWiFi(ssid, pass);

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
}

void loop() {

  if (Serial1.available() > 0) {                /* Check for Serial input pin "Rx" */
    char bfr[5001];
    memset(bfr, 0, 5001);
    Serial1.readBytesUntil('A', bfr, 5000);   /* readBytesUntil(Terminator, data, dataLength) */
    Serial.println(bfr);

    digitalWrite(32, 1);                      /* Blink RGB Red on data recieve from Rx */
    delay(300);
    digitalWrite(32, 0);

    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)) {
      Serial.println("Succeed");
      digitalWrite(15, 1);                      /* Blink RGB Green Led on data send through mqtt */
      delay(300);
      digitalWrite(15, 0);
      delay(300);
      Serial1.write("1");                       /* To clear SD Card a signal is sent through ESP32`s TX1 to PLC */
    } else {
      Serial.println("Failure");
      digitalWrite(33, 1);                      /* Blink RGB Blue Led on data send through mqtt */
      delay(300);
      digitalWrite(33, 0);
      delay(300);
      ESP.restart();                            /* Restrart esp32 if there is a failure in send request */
    }
  }
}

void connectToWiFi(const char * ssid, const char * pwd) {
  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);                 /* Blink LED while connecting to WiFi */
    ledState = (ledState + 1) % 2;              /* Flip LED State */
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 60000) {
      ESP.restart();
    }
  }

  digitalWrite(14, 1);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
