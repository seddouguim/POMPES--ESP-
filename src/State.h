#ifndef SRC_STATE
#define SRC_STATE

#include <Arduino.h>
#include <Adafruit_MAX31855.h>
#include "config.h"

class State
{
public:
    State();
    void update();

private:
    Adafruit_MAX31855 Thermocouple;

    float current_temperature;
    float previous_temperature;

    bool resistance_state;
    bool previous_resistance_state;

    bool pump_state;
    bool previous_pump_state;

    void init();

    friend class Cycle;
    friend class Manager;
    friend class ActionHandler;
    friend class Network;
};

#endif /* SRC_STATE */
