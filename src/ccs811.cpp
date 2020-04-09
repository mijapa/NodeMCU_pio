#include "ccs811.h"
#include "Adafruit_CCS811.h"
#include <Wire.h>
#include <SPI.h>

Adafruit_CCS811 ccs;

void check_if_exist_I2C_() {
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

void setup_ccs811() {
    Serial.begin(115200);
//    Wire.begin(D2, D3);
//    delay(100);
//    check_if_exist_I2C_();
    while (!ccs.begin()) {
        Serial.println("Failed to start sensor! Please check your wiring.");
    }
}

double read_ccs811_eco2() {
    if (ccs.available()) {
        if (!ccs.readData()) {
            return ccs.geteCO2();
        }
    }
    return -10;
}

double read_ccs811_tvoc() {
    return ccs.getTVOC();
}

double read_ccs811_temp() {
    return ccs.calculateTemperature();
}