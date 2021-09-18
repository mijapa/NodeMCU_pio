#define ARDUINOJSON_USE_LONG_LONG 1
#define LARGE_JSON_DOCUMENT_SIZE 8192
#define SMALL_JSON_DOCUMENT_SIZE 4096

#include "WebThingAdapter.h"
#include "bmp280_webthing.h"
#include "bmp280.h"
#include "dht.h"
#include "dallas.h"

const char *bmp280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("bmp280", "BMP280 Weather Sensor", bmp280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty weatherPres("pressure", "", NUMBER, nullptr);
ThingProperty humiditySensorProperty("humidity", "Humidity",
                                     NUMBER, "LevelProperty");
ThingProperty dhttemp("room_temp", "Room Temperature",
                      NUMBER, "TemperatureProperty");

const char *dallasTypes[] = {"TemperatureSensor", nullptr};
ThingDevice dallasFloor("dallasFloor", "Dallas Floor Temperature Sensors", dallasTypes);
//ThingProperty dallastemp0("floor_temp0", "Floor Temperature 0",
//                          NUMBER, "TemperatureProperty");
ThingProperty dallastemp1("floor_temp1", "Floor Temperature 1",
                          NUMBER, "TemperatureProperty");

ThingDevice dallasBasement("dallasBasement", "Dallas Basement Temperature Sensors", dallasTypes);

ThingProperty dallastemp2("floor_temp2", "Floor Temperature 2",
                          NUMBER, "TemperatureProperty");
ThingProperty dallastemp3("floor_temp3", "Floor Temperature 3",
                          NUMBER, "TemperatureProperty");
ThingProperty dallastemp4("floor_temp4", "Floor Temperature 4",
                          NUMBER, "TemperatureProperty");
ThingProperty dallastemp5("floor_temp5", "Floor Temperature 5",
                          NUMBER, "TemperatureProperty");
ThingProperty dallastemp6("floor_temp6", "Floor Temperature 6",
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
    humiditySensorProperty.title = "Wilgotność";
    weather.addProperty(&humiditySensorProperty);

    dhttemp.title = "Temperatura";
    dhttemp.multipleOf = 0.1;
    weather.addProperty(&dhttemp);

    adapter->addDevice(&weather);

//    dallastemp0.title = "Floor Temperature 0";
//    dallastemp0.multipleOf = 0.1;
//    dallasFloor.addProperty(&dallastemp0);
    dallastemp1.title = "Podłoga 1";
    dallastemp1.multipleOf = 0.1;
    dallasFloor.addProperty(&dallastemp1);
    dallastemp2.title = "Podłoga 2";
    dallastemp2.multipleOf = 0.1;
    dallasFloor.addProperty(&dallastemp2);

    adapter->addDevice(&dallasFloor);

    dallastemp3.title = "Zasilanie podłogówki";
    dallastemp3.multipleOf = 0.1;
    dallasBasement.addProperty(&dallastemp3);
    dallastemp4.title = "Powrót podłogówki";
    dallastemp4.multipleOf = 0.1;
    dallasBasement.addProperty(&dallastemp4);
    dallastemp5.title = "Zasilanie C.O.";
    dallastemp5.multipleOf = 0.1;
    dallasBasement.addProperty(&dallastemp5);
    dallastemp6.title = "Powrót C.O.";
    dallastemp6.multipleOf = 0.1;
    dallasBasement.addProperty(&dallastemp6);

    adapter->addDevice(&dallasBasement);
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

//    value.number = getdallastemp(0);
//    dallastemp0.setValue(value);
    value.number = getdallastemp(0);
    dallastemp1.setValue(value);
    value.number = getdallastemp(1);
    dallastemp2.setValue(value);
    value.number = getdallastemp(2);
    dallastemp3.setValue(value);
    value.number = getdallastemp(3);
    dallastemp4.setValue(value);
    value.number = getdallastemp(4);
    dallastemp5.setValue(value);
    value.number = getdallastemp(5);
    dallastemp6.setValue(value);

    adapter->update();
}
