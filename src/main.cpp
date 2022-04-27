#include <Arduino.h>
#include <ESP8266WebServer.h>

#define _ESPASYNC_WIFIMGR_LOGLEVEL_ 4

#ifdef ESP32
#include <Preferences.h>
#include <LITTLEFS.h>

#elif defined(ESP8266)
#include "LittleFS.h"
#define LITTLEFS LittleFS
#endif

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif
const char *ssid = STASSID;
const char *password = STAPSK;



// From onewire lib
#if defined(ARDUINO_ARCH_ESP8266)
// Special note: I depend on the ESP community to maintain these definitions and
// submit good pull requests.  I can not answer any ESP questions or help you
// resolve any problems related to ESP chips.  Please do not contact me and please
// DO NOT CREATE GITHUB ISSUES for ESP support.  All ESP questions must be asked
// on ESP community forums.
#define PIN_TO_BASEREG(pin)             ((volatile uint32_t*) GPO)
#define PIN_TO_BITMASK(pin)             (1 << pin)
#define IO_REG_TYPE uint32_t
#define IO_REG_BASE_ATTR
#define IO_REG_MASK_ATTR
#define DIRECT_READ(base, mask)         ((GPI & (mask)) ? 1 : 0)    //GPIO_IN_ADDRESS
#define DIRECT_MODE_INPUT(base, mask)   (GPE &= ~(mask))            //GPIO_ENABLE_W1TC_ADDRESS
#define DIRECT_MODE_OUTPUT(base, mask)  (GPE |= (mask))             //GPIO_ENABLE_W1TS_ADDRESS
#define DIRECT_WRITE_LOW(base, mask)    (GPOC = (mask))             //GPIO_OUT_W1TC_ADDRESS
#define DIRECT_WRITE_HIGH(base, mask)   (GPOS = (mask))             //GPIO_OUT_W1TS_ADDRESS
#endif

ESP8266WebServer server(80);

uint8_t pin = 12;
    IO_REG_TYPE bitmask;
    volatile IO_REG_TYPE *baseReg;


void handleRoot()
{

    server.send(200, "text/plain", "hello from esp8266!");
}

/* Works, ish

#define SEND_0 \
    DIRECT_WRITE_HIGH(reg, mask); \
	delayMicroseconds(364); \
    DIRECT_WRITE_LOW(reg, mask); \
    delayMicroseconds(728); \

#define SEND_1 \
    DIRECT_WRITE_HIGH(reg, mask); \
	delayMicroseconds(364); \
    DIRECT_WRITE_LOW(reg, mask); \
    delayMicroseconds(321); \
    DIRECT_WRITE_HIGH(reg, mask); \
    delayMicroseconds(728-321); \

*/

#define PERIOD 1000
#define SINGLE_PERIOD (PERIOD/3)
#define DOUBLE_PERIOD ((PERIOD*2)/3)

#define SEND_0 \
    DIRECT_WRITE_HIGH(reg, mask); \
	delayMicroseconds(SINGLE_PERIOD); \
    DIRECT_WRITE_LOW(reg, mask); \
    delayMicroseconds(DOUBLE_PERIOD); \

#define SEND_1 \
    DIRECT_WRITE_HIGH(reg, mask); \
	delayMicroseconds(SINGLE_PERIOD); \
    DIRECT_WRITE_LOW(reg, mask); \
    delayMicroseconds(SINGLE_PERIOD); \
    DIRECT_WRITE_HIGH(reg, mask); \
    delayMicroseconds(SINGLE_PERIOD); \


#define SEND_PREAMBLE \
    SEND_0 \
    SEND_0 \
    SEND_0 \
    SEND_0 \
    SEND_1 \
    SEND_0 \
    SEND_1 \
    SEND_0 \
    SEND_0 \
    SEND_0 \
    SEND_1 \
    SEND_0

void handleSendLight()
{
IO_REG_TYPE mask IO_REG_MASK_ATTR = bitmask;
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = baseReg;

    DIRECT_WRITE_LOW(reg, mask); 

    for(int i=0;i<5;i++) {
        noInterrupts();
	
        SEND_PREAMBLE

        SEND_0
        SEND_0
        SEND_1
        SEND_0

         SEND_1
        SEND_0
        SEND_0
    
        DIRECT_WRITE_LOW(reg, mask); 

        delayMicroseconds(16000); // 16ms gap

        interrupts();
    }

    server.send(200, "text/plain", "OK");
}

void handleSendFanUp()
{
IO_REG_TYPE mask IO_REG_MASK_ATTR = bitmask;
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = baseReg;

    DIRECT_WRITE_LOW(reg, mask); 

    for(int i=0;i<5;i++) {
        noInterrupts();
	
        SEND_PREAMBLE

        SEND_1
        SEND_0
        SEND_1
        SEND_0

         SEND_0
        SEND_0
        SEND_0
    
        DIRECT_WRITE_LOW(reg, mask); 

        delayMicroseconds(16000); // 16ms gap

        interrupts();
    }

    server.send(200, "text/plain", "OK");
}


void handleSendFanDown()
{
IO_REG_TYPE mask IO_REG_MASK_ATTR = bitmask;
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = baseReg;

    DIRECT_WRITE_LOW(reg, mask); 

    for(int i=0;i<5;i++) {
        noInterrupts();
	
        SEND_PREAMBLE

        SEND_1
        SEND_0
        SEND_1
        SEND_1

        SEND_1
        SEND_1
        SEND_0
    
        DIRECT_WRITE_LOW(reg, mask); 

        delayMicroseconds(16000); // 16ms gap

        interrupts();
    }

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
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // setup pin
    pinMode(pin, OUTPUT);
	bitmask = PIN_TO_BITMASK(pin);
	baseReg = PIN_TO_BASEREG(pin);

    // Server
    server.on("/", handleRoot);
    server.on("/light", handleSendLight);
    server.on("/fan/up", handleSendFanUp);
    server.on("/fan/down", handleSendFanDown);
    

    server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
}