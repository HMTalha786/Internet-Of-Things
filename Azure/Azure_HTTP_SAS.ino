#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// ==================================================================================================================================================================================================

const char* ssid = "Realme";

const char* pass = "2k17mte04";

// ==================================================================================================================================================================================================

String Line_ID = "ebm-line-1_0ybel";

String Primary_Key = "aT1HGfu+U/fKHzovuFRtM3fG0Wy443sWCicMfVXwa9o=";

// ==================================================================================================================================================================================================

String SAS_Token;

String SAS_Signature;

// ==================================================================================================================================================================================================

String SAS_Generation_URL = "https://procheckprodfunctions.azurewebsites.net/api/GetSasToken?code=4ji8Wv3sHFDHRaoGgcjhzZos6IaWZmwgdUW2POuVUtBXRBY5%2F0MbSw%3D%3D";

// ==================================================================================================================================================================================================

String Azure_URL = "https://procheck-prod.azure-devices.net/devices/" + Line_ID + "/messages/events?api-version=2020-03-13";

// ==================================================================================================================================================================================================

const char* body = "{\"device\":\"357073299541579\",\"temp\":\"24.6\",\"hum\":\"41.25\",\"bat\":\"41.54\"}";

// ==================================================================================================================================================================================================

void setup() {

  Serial.begin(500000);

  connect_to_wifi();

  fetch_sas_token();

}

// ==================================================================================================================================================================================================

void loop() {

  if (WiFi.status() == WL_CONNECTED) {                         // Check WiFi connection status

    HTTPClient http;
    http.begin(Azure_URL);                                     // Specify destination for HTTP request

    http.addHeader("Content-Type", "application/json");        // Specify content-type header
    http.addHeader("Authorization", SAS_Signature);

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

// ==================================================================================================================================================================================================

void fetch_sas_token() {

  /* Create HTTP Client Instance */
  HTTPClient http;
  http.begin(SAS_Generation_URL);
  http.addHeader("Content-Type", "application/json");

  /*Create JSON Request Body */
  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["id"] = Line_ID;
  JSON_Entry["key"] = Primary_Key;

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  String request_body = JSONmessageBuffer;
  Serial.print("Request Body for fetching SAS Token : ");
  Serial.println(request_body);

  /* Make HTTP Post Request to fetch SAS Token */
  int httpResponseCode = http.POST(request_body);

  /* Handle HTTP Response i.e. Payload which is SAS Token */
  if (httpResponseCode > 0) {
    String response = http.getString();                      // Get the response to the request
    Serial.println(httpResponseCode);                        // Print return code
    Serial.println(response);                                // Print request answer

    /* Fetched SAS Token */
    SAS_Token = String(response);
    Serial.print("SAS_Token : ");
    Serial.println(SAS_Token);

    /* Whole SAS Signature for HTTP Authorization */
    SAS_Signature = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2F" + Line_ID + "&sig=" + SAS_Token;
    Serial.print("SAS_Signature : ");
    Serial.println(SAS_Signature);

    /*Azure IoT Hub HTTP Endpoint for making Post Request*/
    Serial.print("Azure IoT Hub Url : ");
    Serial.println(Azure_URL);

  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// ==================================================================================================================================================================================================

void connect_to_wifi() {

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {           
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

}
