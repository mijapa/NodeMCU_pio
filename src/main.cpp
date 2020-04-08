#include <Arduino.h>
#include "bmp280.h"
#include "ota.h"
#include "webthings.h"
#include "wifiManager.h"


#define BUILTIN_LED D4

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

void setup() {
    Serial.begin(115200);
    Serial.println();
    setupWiFi();
    setupOTA();
    setup_bmp280();
    setupWebthings();
}

void loop() {
    loopOTA();
    loopWebthings();
    delay(100);
}
