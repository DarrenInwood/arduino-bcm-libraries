#ifndef VEHICLE_H
#define VEHICLE_H

// Gives us named indexes in the vehicle state arrays
#include "Arduino.h"
#include "VehicleDefs.h"

// The Vehicle state machine.

class Vehicle {
  public:
    // The state of the vehicle inputs
    bool switches[SW_COUNT];
    // Analog values
    uint16_t values[VAL_COUNT];
    // Set state of a switch
    void setSwitch(uint8_t, bool);
    // Set state of a value
    void setValue(uint8_t, uint16_t);
};

#endif
