#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Cotbus";
const char* pass = "Cotbus@123";

const char* url =  "https://Cotbus-prod.azure-devices.net/devices/testingnow_avi63/messages/events?api-version=2020-03-13";
const char* sas =  "SharedAccessSignature sr=Cotbus-prod.azure-devices.net%2Fdevices%2Ftestingnow_avi63&sig=%2Bwgm%2BuRpHnN5jn8px4916zLWMUUxcWjHgmHJcXGH8bQ%3D&se=1685168670";

const char* body = "{\"device\":\"357073299541579\",\"temp\":\"24.6\",\"hum\":\"41.25\",\"bat\":\"41.54\"}";
void setup() {

  Serial.begin(500000);
  delay(4000);   //Delay needed before calling the WiFi.begin

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;
    http.begin(url);                                           // Specify destination for HTTP request

    http.addHeader("Content-Type", "application/json");        // Specify content-type header
    http.addHeader("Authorization", sas);

    int httpResponseCode = http.POST(body);                    // Send the actual POST request

    if (httpResponseCode > 0) {
      String response = http.getString();                      // Get the response to the request
      Serial.println(httpResponseCode);                        // Print return code
      Serial.println(response);                                // Print request answer
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();                                                // Free resources

  }

  delay(10000);                                                // Send a request every 10 seconds
}
