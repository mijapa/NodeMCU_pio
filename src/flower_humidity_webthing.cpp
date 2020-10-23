#define ARDUINOJSON_USE_LONG_LONG 1
#include "WebThingAdapter.h"
#include "flower_humidity_webthing.h"
#include "flower_humidity.h"

const char *flow_humTypes[] = {"MultiLevelSensor", nullptr};
ThingDevice flow_hum("flowhum", "Flower Humidity Sensor", flow_humTypes);
ThingProperty flow_humidity("humidity", "Humidity", NUMBER, "LevelProperty");

void setup_webthing_flower_humidity(WebThingAdapter *adapter) {
    flow_humidity.unit = "percent";
    flow_humidity.readOnly = "true";
    flow_humidity.multipleOf = 0.1;
    flow_humidity.title = "Humidity";
    flow_hum.addProperty(&flow_humidity);
    adapter->addDevice(&flow_hum);
}

void loop_webthing_flower_humidity(WebThingAdapter *adapter) {
    ThingPropertyValue value;
    value.number = read_flowHum_humidity();
    flow_humidity.setValue(value);
    adapter->update();
}