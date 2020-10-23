#define ARDUINOJSON_USE_LONG_LONG 1
#define LARGE_JSON_BUFFERS 1
#include "webthings.h"
#include "bmp280_webthing.h"
#include "flower_humidity_webthing.h"
#include <Arduino.h>

WebThingAdapter *adapter;

void setupWebthings() {
    adapter = new WebThingAdapter("NodeMCU1", WiFi.localIP());
    setup_webthing_bmp280(adapter);

    adapter->begin();
    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/things/");
    Serial.print("\n\n");
}

void loopWebthings() {
    loop_webthing_bmp280(adapter);
}

