#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "bmp280.h"

Adafruit_BMP280 bmp; // I2C

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

void setup_bmp280() {
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

double read_bmp280_temp() {
    return bmp.readTemperature();
}

double read_bmp280_pressure() {
    return bmp.readPressure() / 100;
}