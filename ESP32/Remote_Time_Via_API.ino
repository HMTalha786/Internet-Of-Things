#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "wifi name";
const char* pass = "wifi password";

int timer;

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi ...");
  }
  Serial.println("Connected to the WiFi network");
  get_time();
}

void loop() {
  Serial.println("Hello");
  delay(timer);
}

void get_time() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("https://f0149be4-0e2e-41e1-936d-645f62f5f026.mock.pstmn.io/");   // Specify the URL
    int httpCode = http.GET();                                                   // Make the request

    //Check for the returning code

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      // Int conversion
      timer = payload.toInt();
      Serial.print("Time set to : ");
      Serial.println(timer);
    }

    else {
      Serial.println("Error on HTTP request");
      timer = 10000;
    }

    http.end(); //Free the resources
  }
}
