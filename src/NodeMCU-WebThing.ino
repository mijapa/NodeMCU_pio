#include "DHTesp.h"
#include <Ticker.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include "setupWiFi.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <user_config.h>
#include <ESPAsyncWebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <DNSServer.h>

#define DHTPIN D5 // Pin which is connected to the DHT sensor.
#define PIN_PIXELS D3 //Pin which is connected to the NeoPixels strip.
#define NUM_PIXELS 8
#define DQ7_PIN 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);


#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

DHTesp dht;
Ticker sampler;
boolean isTimeToSample = false;

WebThingAdapter *adapter = NULL;

const char *sensorTypes[] = {"TemperatureSensor", "MultiLevelSensor", "Sensor", nullptr};
ThingDevice dhtSensor("dht4", "DHT22 Temperature & Humidity sensor", sensorTypes);
ThingProperty tempSensorProperty("temperature", "Temperature", NUMBER, "TemperatureProperty");
ThingProperty humiditySensorProperty("humidity", "Humidity", NUMBER, "LevelProperty");

const char *ledStripTypes[] = {"Light", "OnOffSwitch", "ColorControl", nullptr};
ThingDevice ledStrip("strip", "Dimmable Color Light", ledStripTypes);
ThingProperty ledStripOn("on", "Whether the led is turned on", BOOLEAN, "OnOffProperty");
ThingProperty ledStripLevel("level", "The level of light from 0-100", NUMBER, "BrightnessProperty");
ThingProperty ledStripColor("color", "The color of light in RGB", STRING, "ColorProperty");

const char *DQ7sensorTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice DQ7Sensor("dq7", "CO gas sensor", DQ7sensorTypes);
ThingProperty gasSensorProperty("gas", "CO gas level", NUMBER, "LevelProperty");

bool lastOn = false;
String color = "#ffffff";

AsyncWebServer server(80);
DNSServer dns;

void dhtHandle();

void stripHandle();

void dq7Handle();

void sample() {
    isTimeToSample = true;
}

void setupDHT() {
    dht.setup(DHTPIN, DHTesp::DHT22);
    sampler.attach_ms(dht.getMinimumSamplingPeriod(), sample);
}

void webThingSetup() {
    adapter = new WebThingAdapter("NodeMCU1", WiFi.localIP());

    dhtSensor.addProperty(&tempSensorProperty);
    dhtSensor.addProperty(&humiditySensorProperty);
    adapter->addDevice(&dhtSensor);

    ledStrip.addProperty(&ledStripOn);

    ThingPropertyValue colorValue;
    colorValue.string = &color; //default color is white
    ledStripColor.setValue(colorValue);
    ledStrip.addProperty(&ledStripColor);

    ThingPropertyValue levelValue;
    levelValue.number = 100;
    ledStripLevel.setValue(levelValue);
    ledStrip.addProperty(&ledStripLevel);
    adapter->addDevice(&ledStrip);

    DQ7Sensor.addProperty(&gasSensorProperty);
    adapter->addDevice(&DQ7Sensor);

    adapter->begin();
    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/things/");
    Serial.println(dhtSensor.id);
}

void otaSetup() {
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword((const char *)"123");

    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        ESP.restart();
    });

    ArduinoOTA.begin();
    Serial.println("OTA ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

/**
   hex2int
   take a 2 digit hex string and convert it to a integer
*/
int twoHex2int(String hex) {
    int len = 2;
    int i;
    int val = 0;

    for (i = 0; i < len; i++) {
        if (hex[i] <= '9')
            val += (hex[i] - '0') * (1 << (4 * (len - 1 - i)));
        else if (hex[i] <= 'F')
            val += (hex[i] - 'A' + 10) * (1 << (4 * (len - 1 - i)));
        else
            val += (hex[i] - 'a' + 10) * (1 << (4 * (len - 1 - i)));
    }
    return val;
}

void updateLedStrip(String *color, int const level) {
    if (!color) return;
    int red = 0, green = 0, blue = 0;
    if ((color->length() == 7) && color->charAt(0) == '#') {
        red = twoHex2int(color->substring(1, 3));
        green = twoHex2int(color->substring(3, 5));
        blue = twoHex2int(color->substring(5, 7));
    }
    for (int i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(red, green, blue));
        pixels.setBrightness(level);
        pixels.show();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    setupDHT();
    setupWiFi();
    otaSetup();
    webThingSetup();
    pixels.begin();
}

void loop() {
    ArduinoOTA.handle();
    dq7Handle();
    dhtHandle();
    stripHandle();
    adapter->update();
}

void dq7Handle() {
    float gas = analogRead(DQ7_PIN);
    if (isTimeToSample) {
        Serial.println("Gas: ");
        Serial.print(gas);
    }
    ThingPropertyValue gasValue;
    gasValue.number = gas;
    gasSensorProperty.setValue(gasValue);
    delay(500);
}

void stripHandle() {
    bool on = ledStripOn.getValue().boolean;
    int level = ledStripLevel.getValue().number;
    updateLedStrip(&color, on ? level : 0);

    if (on != lastOn) {
        Serial.print(ledStrip.id);
        Serial.print(": on: ");
        Serial.print(on);
        Serial.print(", level: ");
        Serial.print(level);
        Serial.print(", color: ");
        Serial.println(color);
    }
    lastOn = on;
}

void dhtHandle() {
    if (isTimeToSample) {
        float humidity = dht.getHumidity();
        float temperature = dht.getTemperature();
        Serial.println("Status\tHumidity (%)\tTemperature (C)\tHeatIndex (C)");
        Serial.print(dht.getStatusString());
        Serial.print("\t");
        Serial.print(humidity, 1);
        Serial.print("\t\t");
        Serial.print(temperature, 1);
        Serial.print("\t\t");
        Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
        Serial.print("\n");
        ThingPropertyValue tempValue;
        tempValue.number = temperature;
        tempSensorProperty.setValue(tempValue);
        ThingPropertyValue humidityValue;
        humidityValue.number = humidity;
        humiditySensorProperty.setValue(humidityValue);
        isTimeToSample = false;
    }
}
