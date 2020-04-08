#include "webthings.h"
#include "bmp280_webthing.h"
#include "flower_humidity_webthing.h"
#include <Arduino.h>

WebThingAdapter *adapter;

void setupWebthings() {
    adapter = new WebThingAdapter("NodeMCU1", WiFi.localIP());
    setup_webthing_flower_humidity(adapter);
    setup_webthing_bmp280(adapter);
}

void loopWebthings() {
    loop_webthing_bmp280(adapter);
    loop_webthing_flower_humidity(adapter);
}

