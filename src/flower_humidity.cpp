#include "flower_humidity.h"
#include <Arduino.h>

double read_flowHum_humidity() {
    return map(analogRead(A0), MAX_ANALOG_READ, MIN_ANALOG_READ, MIN_PERCENT, MAX_PERCENT);
}


