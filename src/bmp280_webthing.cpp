#include "WebThingAdapter.h"
#include "bmp280_webthing.h"
#include "bmp280.h"

const char *bmp280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("bmp280", "BMP280 Weather Sensor", bmp280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty weatherPres("pressure", "", NUMBER, nullptr);

void setup_webthing_bmp280(WebThingAdapter *adapter) {
    weatherTemp.unit = "celsius";
    weatherTemp.multipleOf = 0.1;
    weather.addProperty(&weatherTemp);
    weatherPres.title = "Ciśnienie";
    weatherPres.unit = "hPa";
    weatherPres.multipleOf = 0.01;
    weather.addProperty(&weatherPres);
    adapter->addDevice(&weather);

    adapter->begin();
    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/things/");
    Serial.print("\n\n");
}

void loop_webthing_bmp280(WebThingAdapter *adapter) {
    ThingPropertyValue value;
    value.number = read_bmp280_pressure();
    weatherPres.setValue(value);
    adapter->update();
    value.number = read_bmp280_temp();
    weatherTemp.setValue(value);
    adapter->update();
}
