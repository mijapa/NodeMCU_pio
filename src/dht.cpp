#include <DHTesp.h>
#include <Ticker.h>
#include "dht.h"

#define DHTPIN 5 // Pin which is connected to the DHT sensor.

DHTesp dht;
Ticker updateTicker;
bool updateDHT = false;
double heatIndex = 0;

void timeToUpdateDHT() {
    updateDHT = true;
}

void setupDHT() {
    dht.setup(DHTPIN, DHTesp::DHT22);
    updateTicker.attach_ms(1000, timeToUpdateDHT);

}

void loopDHT() {
    if (updateDHT) {
        updateDHT = false;
        dht.getTempAndHumidity();
        if (dht.getStatus() == 0) {
            if (dht.values.humidity != nanf("") && dht.values.temperature != nanf("")) {
                if (dht.values.humidity > 0 && dht.values.humidity < 100 && dht.values.temperature > -30 &&
                    dht.values.temperature < 30)
                    heatIndex = dht.computeHeatIndex(dht.values.temperature, dht.values.humidity);
            }
        }
    }
}

double getDhtHeatIndex() {
    return heatIndex;
}

double getDHTtemp() {
    return dht.values.temperature;
}

double getDHThumi() {
    return dht.values.humidity;
}
