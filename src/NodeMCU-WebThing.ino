#include <Ticker.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <DNSServer.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BUILTIN_LED D4

Adafruit_BMP280 bmp; // I2C

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif


WebThingAdapter *adapter = NULL;


AsyncWebServer server(80);
DNSServer dns;

Ticker ledTicker;

const char *bmp280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("bmp280", "BMP280 Weather Sensor", bmp280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty weatherPres("pressure", "", NUMBER, nullptr);

void check_if_exist_I2C() {
    byte error, address;
    int nDevices;
    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");
            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    } //for loop
    if (nDevices == 0)
        Serial.println("No I2C devices found");
    else
        Serial.println("**********************************\n");
    //delay(1000);           // wait 1 seconds for next scan, did not find it necessary
}

void bmpSetup() {
    Wire.begin(D2, D3);
    check_if_exist_I2C();
    while (!bmp.begin(0x76)) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}


void blink() {
    //toggle state
    int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
    digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(AsyncWiFiManager *myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
    //entered config mode, make led toggle faster
    ledTicker.attach_ms(100, blink);
}


void setupWiFi() {

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.begin(115200);


    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    AsyncWiFiManager wifiManager(&server, &dns);
    //reset settings - for testing
    //wifiManager.resetSettings();

    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    //fetches ssid and pass and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect()) {
        Serial.println("failed to connect and hit timeout");
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1000);
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WiFi.BSSIDstr());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {// Start the mDNS responder for esp8266.local
        MDNS.addService("http", "tcp", 80);
        MDNS.addServiceTxt("http", "tcp", "url", "http://esp8266.local");
        MDNS.addServiceTxt("http", "tcp", "webthing", "true");
        Serial.println("MDNS responder started");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }
}

void webThingSetup() {
    adapter = new WebThingAdapter("NodeMCU1", WiFi.localIP());

    weatherTemp.unit = "celsius";
    weather.addProperty(&weatherTemp);
    weather.addProperty(&weatherPres);
    adapter->addDevice(&weather);

    adapter->begin();
    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/things/");
    Serial.print("\n\n");
}


void readBME280Data() {
    float temp(NAN), pres(NAN);

    ThingPropertyValue value;
    value.number = bmp.readPressure();
    weatherPres.setValue(value);
    value.number = bmp.readTemperature();
    weatherTemp.setValue(value);
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


void setup() {
    Serial.begin(115200);
    Serial.println();
    setupWiFi();
    otaSetup();
    webThingSetup();
    bmpSetup();
}

void loop() {
    ArduinoOTA.handle();
    readBME280Data();
    adapter->update();
}
