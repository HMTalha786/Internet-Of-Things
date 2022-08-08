#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid = "HomeNet-102";
const char* pass =  "89102828";

IPAddress staticIP(192, 168, 100, 13);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 100, 1);

void setup() {
  Serial.begin(500000);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting...\n\n");
  }

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));

  bool success = Ping.ping("www.google.com", 3);

  if (!success) {
    Serial.println("\nPing failed");
    return;
  } else {
    Serial.println("\nPing successful.");
  }
}

void loop() {
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}
