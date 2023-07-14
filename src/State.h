#ifndef SRC_STATE
#define SRC_STATE

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include "config.h"

class State
{
public:
    State();
    void update();
    char *get_state_json();
    char *get_shadow_update_document();

    bool get_temperature_update();
    bool get_resistance_update();
    bool get_pump_update();

private:
    unsigned long last_update;

    OneWire oneWire;
    DallasTemperature temperature_sensor;

    //* TODO: GET CUID FROM EEPROM
    String CUID = "cuid-01";

    float current_temperature;
    float previous_temperature;

    bool resistance_state;
    bool previous_resistance_state;

    bool pump_state;
    bool previous_pump_state;

    // Variables to keep track of the time the pump and the resistance have been on
    unsigned long pump_start_timer;
    unsigned long resistance_start_timer;

    double pump_kwh;
    double resistance_kwh;

    double daily_pump_kwh;
    double daily_resistance_kwh;
    double monthly_pump_kwh;
    double monthly_resistance_kwh;

    String current_cycle;
    String current_term;

    unsigned long current_cycle_duration;
    unsigned long current_term_duration;

    void calculate_kws();

    void pump_kw();
    void resistance_kw();

    void init();
    float get_temperature();

    friend class Cycle;
    friend class Manager;
    friend class ActionHandler;
    friend class Network;
    friend class Display;
};

#endif /* SRC_STATE */
