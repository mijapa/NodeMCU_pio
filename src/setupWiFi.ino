#include "../../../.platformio/packages/framework-arduinoespressif8266/libraries/Ticker/Ticker.h"
#include "../../../.platformio/packages/framework-arduinoespressif8266/variants/generic/common.h"
#include "../../../.platformio/packages/framework-arduinoespressif8266/cores/esp8266/HardwareSerial.h"
#include "../../../.platformio/packages/framework-arduinoespressif8266/libraries/ESP8266WiFi/src/ESP8266WiFi.h"
#include "../../../.platformio/packages/framework-arduinoespressif8266/libraries/ESP8266mDNS/src/ESP8266mDNS.h"
#include "../../../.platformio/packages/framework-arduinoespressif8266/cores/esp8266/Esp.h"

Ticker ledTicker;

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