#include <SPI.h>
#include <Ethernet.h>

EthernetClient client;

// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
int HTTP_PORT = 80;
String HTTP_METHOD = "GET"; // or POST
char HOST_NAME[] = "eb5f742f-14aa-4093-b05a-b57047794796.mock.pstmn.io";
String PATH_NAME = "/";

void setup() {
  Serial.begin(9600);
  // initialize the Ethernet shield using DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address using DHCP");
    while (true);
  }
}

void loop() {

  // connect to web server on port 80:
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
//        Serial.print(String(c));
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {
    // if not connected:
    Serial.println("connection failed");
  }
    delay(10000);
}
