#include <WiFi.h>
#include "Esp32MQTTClient.h"

const char* ssid     = "HomeNet-102-0311-2051551";
const char* pass     = "Newpassword";
static const char* connectionString = "HostName=<hostname>;DeviceId=<LineId>;SharedAccessKey=<PrimaryKey>";
static bool hasIoTHub = false;

/*======================================================================================================================================================================*/

void setup() {
  Serial.begin(500000);
  Serial.println("Connecting to WiFi...");
  delay(10);
  WiFi.begin(ssid, password);
  delay(10);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }
  Serial.println("WiFi connected Successfully");
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)) {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  Serial.println("Start sending events.");
}

/*======================================================================================================================================================================*/

void loop() {
  if (hasIoTHub) {
    char buff[128]; snprintf(buff, 128, "{\"topic\":\"hello world\"}");       // replace the following line with your data sent to Azure IoTHub
    if (Esp32MQTTClient_SendEvent(buff)) {
      Serial.println("Succeed ...");
    }
    else {
      Serial.println("Failure ...");
    }
    delay(1000);
  }
}
