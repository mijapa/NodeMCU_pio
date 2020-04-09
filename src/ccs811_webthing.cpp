#define ARDUINOJSON_USE_LONG_LONG 1

#include "WebThingAdapter.h"
#include "ccs811_webthing.h"
#include "ccs811.h"

const char *ccs811Types[] = {"AirQualitySensor", "TemperatureSensor", nullptr};
ThingDevice ccs811("ccs811", "CCS811 eCO2 Sensor", ccs811Types);
ThingProperty ccs811temp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty ccs811eco2("eco2", "", NUMBER, "ConcentrationProperty");
ThingProperty ccs811tvoc("tvoc", "", NUMBER, "ConcentrationProperty");

void setup_webthing_ccs811(WebThingAdapter *adapter) {
    ccs811temp.unit = "celsius";
    ccs811temp.multipleOf = 0.1;
    ccs811.addProperty(&ccs811temp);
    ccs811tvoc.title = "TVOC";
    ccs811tvoc.unit = "ppm";
    ccs811.addProperty(&ccs811tvoc);
    ccs811eco2.title = "equivalent CO2";
    ccs811eco2.unit = "ppm";
    ccs811.addProperty(&ccs811eco2);
    adapter->addDevice(&ccs811);


}

void loop_webthing_ccs811(WebThingAdapter *adapter) {
    ThingPropertyValue value;
    value.number = read_ccs811_eco2();
    ccs811eco2.setValue(value);
    adapter->update();
    value.number = read_ccs811_tvoc();
    ccs811tvoc.setValue(value);
    adapter->update();
    value.number = read_ccs811_temp();
    ccs811temp.setValue(value);
    adapter->update();
}
