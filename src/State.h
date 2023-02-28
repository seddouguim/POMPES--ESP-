#ifndef SRC_STATE
#define SRC_STATE

#include <Arduino.h>
#include <Adafruit_MAX31855.h>
#include <ArduinoJson.h>
#include "config.h"

class State
{
public:
    State();
    void update();
    char *get_state_json();
    char *get_shadow_update_document();
    StaticJsonDocument<200> *get_state_doc();

    bool get_temperature_update();
    bool get_resistance_update();
    bool get_pump_update();

private:
    Adafruit_MAX31855 Thermocouple;

    //* TODO: GET CUID FROM EEPROM
    String CUID = "claay207y00083b6qint7umkc";

    float current_temperature;
    float previous_temperature;

    bool resistance_state;
    bool previous_resistance_state;

    bool pump_state;
    bool previous_pump_state;

    // Variables to keep track of the time the pump and the resistance have been on
    unsigned long pump_on_time_total;
    unsigned long resistance_on_time_total;

    unsigned long pump_start_timer;
    unsigned long resistance_start_timer;

    float pump_kwh;
    float resistance_kwh;

    void calculate_kws();

    void pump_kw();
    void resistance_kw();

    void init();

    friend class Cycle;
    friend class Manager;
    friend class ActionHandler;
    friend class Network;
    friend class Display;
};

#endif /* SRC_STATE */
