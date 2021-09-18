#define ARDUINOJSON_USE_LONG_LONG 1
#define LARGE_JSON_DOCUMENT_SIZE 8192
#define SMALL_JSON_DOCUMENT_SIZE 4096

#include "WebThingAdapter.h"
#include "bmp280_webthing.h"
#include "bmp280.h"
#include "dht.h"

const char *bmp280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("bmp280", "BMP280 Weather Sensor", bmp280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty weatherPres("pressure", "", NUMBER, nullptr);
ThingProperty humiditySensorProperty("humidity", "Humidity",
                                     NUMBER, "LevelProperty");
ThingProperty dhttemp("room_temp", "Room Temperature",
                      NUMBER, "TemperatureProperty");


void setup_webthing_bmp280(WebThingAdapter *adapter) {
    weatherTemp.unit = "celsius";
    weatherTemp.multipleOf = 0.1;
    weather.addProperty(&weatherTemp);

    weatherPres.title = "Ciśnienie";
    weatherPres.unit = "hPa";
    weatherPres.multipleOf = 0.01;
    weather.addProperty(&weatherPres);

    humiditySensorProperty.unit = "percent";
    humiditySensorProperty.readOnly = "true";
    humiditySensorProperty.multipleOf = 0.1;
    humiditySensorProperty.title = "Humidity";
    weather.addProperty(&humiditySensorProperty);

    dhttemp.title = "Room Temperature";
    dhttemp.multipleOf = 0.1;
    weather.addProperty(&dhttemp);

    adapter->addDevice(&weather);
}

void loop_webthing_bmp280(WebThingAdapter *adapter) {
    ThingPropertyValue value;
    value.number = read_bmp280_pressure();
    weatherPres.setValue(value);
    value.number = read_bmp280_temp();
    weatherTemp.setValue(value);
    value.number = getDHThumi();
    humiditySensorProperty.setValue(value);
    value.number = getDHTtemp();
    dhttemp.setValue(value);

    adapter->update();
}
