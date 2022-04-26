#include <Arduino.h>
#include <ESP8266WebServer.h>



#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {
  
  server.send(200, "text/plain", "hello from esp8266!");
  
}
void handleSend() {
  
  server.send(200, "text/plain", "OK");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup.. ");

    if (LITTLEFS.begin())
    {
        Serial.println("LittleFS OK");
        Serial.println(
            LITTLEFS.exists("/www/index.html") ? "exists" : "not");
    }
    else
    {
        Serial.println("LittleFS NOT OK");
    }

      WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    

    // Server
    server.on("/", handleRoot);
    server.on("/send1", handleSend);

}

void loop()
{
    server.handleClient();

}