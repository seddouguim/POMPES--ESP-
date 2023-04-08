#include "State.h"

State::State()
    : Thermocouple(MAX6675(MAX_CLK, MAX_CS, MAX_MISO)),
      Thermocouple_max31855(Adafruit_MAX31855(MAX_CLK, MAX_CS, MAX_MISO))
{
    init();
}

void State::init()
{
    if (!DEV)
        Thermocouple_max31855.begin();

    current_temperature = 0;
    previous_temperature = -1;

    resistance_state = false;
    previous_resistance_state = false;

    pump_state = false;
    previous_pump_state = false;

    pump_start_timer = 0;
    resistance_start_timer = 0;

    pump_on_time_total = 0;
    resistance_on_time_total = 0;

    pump_kwh = 0;
    resistance_kwh = 0;

    last_update = 0;

    pinMode(RESISTANCE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
}

void State::update()
{
    if (millis() - last_update < 1000)
        return;

    last_update = millis();

    previous_temperature = current_temperature;
    current_temperature = DEV ? Thermocouple.readCelsius() : Thermocouple_max31855.readCelsius();
    // current_temperature = random(10, 100);

    previous_resistance_state = resistance_state;
    resistance_state = digitalRead(RESISTANCE_PIN);

    previous_pump_state = pump_state;
    pump_state = digitalRead(PUMP_PIN);

    if (resistance_state == HIGH)
        resistance_kwh = RESISTANCE_CONSUMPTION / 1000;
    else
        resistance_kwh = 0;

    if (pump_state == HIGH)
        pump_kwh = PUMP_CONSUMPTION / 1000;
    else
        pump_kwh = 0;

    calculate_kws();
}

char *State::get_state_json()
{
    StaticJsonDocument<200> doc;

    doc["device_id"] = CUID;
    doc["current_temperature"] = current_temperature;
    doc["resistance_state"] = resistance_state;
    doc["pump_state"] = pump_state;
    doc["pump_on_time_total"] = pump_on_time_total;
    doc["resistance_on_time_total"] = resistance_on_time_total;
    doc["pump_kwh"] = pump_kwh;
    doc["resistance_kwh"] = resistance_kwh;

    static char state_json[JSON_BUFFER_SIZE];
    serializeJson(doc, state_json);

    return state_json;
}

char *State::get_shadow_update_document()
{
    StaticJsonDocument<200> doc;

    doc["state"]["reported"]["current_temperature"] = current_temperature;
    doc["state"]["reported"]["resistance_state"] = resistance_state;
    doc["state"]["reported"]["pump_state"] = pump_state;
    doc["state"]["reported"]["pump_kwh"] = pump_kwh;
    doc["state"]["reported"]["resistance_kwh"] = resistance_kwh;

    static char shadow_update_document[JSON_BUFFER_SIZE];
    serializeJson(doc, shadow_update_document);

    return shadow_update_document;
}

void State::calculate_kws()
{
    pump_kw();
    resistance_kw();
}

void State::pump_kw()
{
    if (pump_state == previous_pump_state)
        return;

    // Pump has just turned ON
    // We start the pump timer
    if (pump_state)
        pump_start_timer = millis();

    // Pump has just turned OFF
    // We calculate the time the PUMP has been on
    else
    {
        unsigned long pump_on_time_interval = millis() - pump_start_timer;
        pump_on_time_total += pump_on_time_interval;

        // We calculate the number of kilowatt-hours
        pump_kwh = (pump_on_time_total / (3600 * 1000)) * PUMP_CONSUMPTION;
    }
}

void State::resistance_kw()
{
    if (resistance_state == previous_resistance_state)
        return;

    // Resistance has just turned ON
    // We start the resistance timer
    if (resistance_state)
        resistance_start_timer = millis();

    // Resistance has just turned OFF
    // We calculate the time the resistance has been on
    else
    {
        unsigned long resistance_on_time_interval = millis() - resistance_start_timer;
        resistance_on_time_total += resistance_on_time_interval;

        // We calculate the number of kilowatt-hours
        resistance_kwh = (resistance_on_time_total / (3600 * 1000)) * RESISTANCE_CONSUMPTION;
    }
}

// GET_UPATE FUNCTIONS
// RETURN TRUE IF EACH PREVIOUS STATE IS DIFFERENT FROM CURRENT STATE
bool State::get_temperature_update()
{
    return current_temperature != previous_temperature;
}

bool State::get_resistance_update()
{
    return resistance_state != previous_resistance_state;
}

bool State::get_pump_update()
{
    return pump_state != previous_pump_state;
}
