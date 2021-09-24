#include "DHTesp.h"
#include <Ticker.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include <ArduinoOTA.h>
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <DNSServer.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#define BUILTIN_LED 13
#define SWITCH_PIN 12
#define BUTTON_PIN 0

WebThingAdapter *adapter = NULL;
Ticker ledTicker;

const char *switchTypes[] = {"OnOffSwitch", nullptr};
ThingDevice sonoffSwitch("switch", "Sonoff RF R2 Power Switch", switchTypes);
ThingProperty switchOn("on", "Whether the switch is turned on", BOOLEAN, "OnOffProperty");

boolean isOn = false;
boolean isKeyActive = true;
Ticker keyActivator;

AsyncWebServer server(80);
DNSServer dns;

void activateKey() {
    isKeyActive = true;
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
    adapter = new WebThingAdapter("Sonoff", WiFi.localIP());

    sonoffSwitch.addProperty(&switchOn);
    adapter->addDevice(&sonoffSwitch);
    adapter->begin();

    Serial.println("HTTP server started");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/things/");
    Serial.println(sonoffSwitch.id);
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

void switchHandle(){
    digitalWrite(SWITCH_PIN, switchOn.getValue().boolean);
    digitalWrite(BUILTIN_LED, !switchOn.getValue().boolean);
    if (isKeyActive && !digitalRead(BUTTON_PIN)) {
        isOn = !isOn;
        ThingPropertyValue onValue;
        onValue.boolean = isOn;
        switchOn.setValue(onValue);
        isKeyActive = false;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    keyActivator.attach_ms(1000, activateKey);
    pinMode(SWITCH_PIN, OUTPUT);
    setupWiFi();
    otaSetup();
    webThingSetup();
}

void loop() {
    ArduinoOTA.handle();
    switchHandle();
    adapter->update();
}

