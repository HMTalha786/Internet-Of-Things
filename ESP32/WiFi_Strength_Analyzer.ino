#include <WiFi.h>

// Replace with your network credentials (STATION)
const char* ssid = "Studio Phi";
const char* pass = "StudioPhi124";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(500000);
  initWiFi();
}

void loop() {
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println("db");
  delay(500);
}
