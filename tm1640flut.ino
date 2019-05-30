#include "Arduino.h"

#include <WEMOS_Matrix_LED.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#define PIXELFLUT_PORT  1234

static MLED mled(5);
static WiFiManager wifiManager;
static WiFiClient wifiClient;
static WiFiUDP wifiUdp;

void setup(void)
{
    Serial.begin(115200);
    Serial.print("\nESP-FLUT\n");

    // connect to wifi
    Serial.println("Starting WIFI manager ...");
#if 1
    wifiManager.setConfigPortalTimeout(120);
    wifiManager.autoConnect("ESP-FLUT");
#else
    WiFi.begin("Freifunk-disabled");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
#endif

    if (wifiUdp.begin(PIXELFLUT_PORT)) {
        Serial.printf("Opened UDP port %d\n", PIXELFLUT_PORT);
    } else {
        Serial.printf("Failed to open UDP port %d\n", PIXELFLUT_PORT);
    }
}

static void process_line(char *line)
{
    Serial.printf("Line: %s\n", line);
}

static void process_buffer(char *buf, int len)
{
    Serial.println("Buffer:");
    char *p = buf;
    for (int i = 0; i < len; i++) {
        char c = buf[i];
        if (c == '\n') {
            buf[i] = '\0';
            process_line(p);
            p = &buf[i + 1];
        }
    }
}

void loop(void)
{
    static char buffer[1500];

    int size = wifiUdp.parsePacket();
    if (size > 0) {
        int len = wifiUdp.read(buffer, sizeof(buffer));
        process_buffer(buffer, len);

        mled.display();
    }
}
