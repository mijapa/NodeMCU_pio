#include "dallas.h"
#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
//DeviceAddress insideThermometer, outsideThermometer;

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
DeviceAddress DallasThermometers[DALLAS_TEMP_COUNT] = {
//        {0x28, 0x30, 0xD3, 0x00, 0x00, 0x00, 0x00, 0xE3},
        {0x28, 0x68, 0x08, 0x01, 0x00, 0x00, 0x00, 0xCD},
        {0x28, 0x2A, 0x45, 0x01, 0x00, 0x00, 0x00, 0xD3},
        {0x28, 0xA6, 0x07, 0x01, 0x00, 0x00, 0x00, 0x2E},
        {0x28, 0xE1, 0xBE, 0x00, 0x00, 0x00, 0x00, 0xED},
        {0x28, 0xA7, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x1D},
        {0x28, 0xFF, 0x64, 0x1E, 0x0F, 0x0A, 0x08, 0x11}
};

float tempC[DALLAS_TEMP_COUNT] = {100};

uint8_t findDevices(int pin) {
    OneWire ow(pin);

    uint8_t address[8];
    uint8_t count = 0;

    Serial.println("Looking for devices");
    if (ow.search(address)) {
        Serial.print("\nuint8_t pin");
        Serial.print(pin, DEC);
        Serial.println("[][8] = {");
        do {
            count++;
            Serial.println("  {");
            for (uint8_t i = 0; i < 8; i++) {
                Serial.print("0x");
                if (address[i] < 0x10) Serial.print("0");
                Serial.print(address[i], HEX);
                if (i < 7) Serial.print(", ");
            }
            Serial.println("  },");
        } while (ow.search(address));

        Serial.println("};");
        Serial.print("// nr devices found: ");
        Serial.println(count);
    }

    return count;
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
    for (uint8_t i = 0; i < 8; i++) {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress) {
    float tempC = sensors.getTempC(deviceAddress);
    if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Could not read temperature data");
        return;
    }
    Serial.print("Temp C: ");
    Serial.print(tempC);
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress) {
    Serial.print("Resolution: ");
    Serial.print(sensors.getResolution(deviceAddress));
    Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress) {
    Serial.print("Device Address: ");
    printAddress(deviceAddress);
    Serial.print(" ");
    printTemperature(deviceAddress);
    Serial.println();
}

void setupDallas() {
    findDevices(ONE_WIRE_BUS);
    // Start up the library
    sensors.begin();

    // locate devices on the bus
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");


    for (int i = 0; i < DALLAS_TEMP_COUNT; i++) {
        // set the resolution to x bit per device
        sensors.setResolution(DallasThermometers[i], TEMPERATURE_PRECISION);

        Serial.print("Device ");
        Serial.print(i);
        Serial.print(" Resolution: ");
        Serial.print(sensors.getResolution(DallasThermometers[i]), DEC);
        Serial.println();
    }
};

void loopDallas() {
//        findDevices(ONE_WIRE_BUS);
//
//     call sensors.requestTemperatures() to issue a global temperature
//     request to all devices on the bus
//    Serial.print("Requesting temperatures...");
//    sensors.requestTemperatures();
//    Serial.println("DONE");

    sensors.requestTemperatures();
    for (int i = 0; i < DALLAS_TEMP_COUNT; i++) {
        tempC[i] = sensors.getTempC(DallasThermometers[i]);
        if (tempC[i] == DEVICE_DISCONNECTED_C) {
            Serial.println("Error: Could not read temperature data");
        }
//        Serial.print("Thermometer ");
//        Serial.print(i);
//        Serial.print(":");
//        Serial.println(tempC[i]);
    }
};

float getdallastemp(int i) {
    return tempC[i];
}