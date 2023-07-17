#include "State.h"

State::State()
    : oneWire(ds18b20_pin), temperature_sensor(&oneWire)
{
    init();
}

void State::init()
{
    current_temperature = 0;
    previous_temperature = 0;

    resistance_state = false;
    previous_resistance_state = false;

    pump_state = false;
    previous_pump_state = false;

    pump_start_timer = 0;
    resistance_start_timer = 0;

    daily_pump_kwh = 0;
    daily_resistance_kwh = 0;
    monthly_pump_kwh = 0;
    monthly_resistance_kwh = 0;

    pump_kwh = 0;
    resistance_kwh = 0;

    current_cycle = "";
    current_term = "";

    last_update = 0;

    pinMode(RESISTANCE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
}

float State::get_temperature()
{
    static int attempts = 0;
    static float temperature = -127.0;

    temperature_sensor.requestTemperatures();

    while (temperature == -127.0 && attempts < 10)
    {
        temperature = temperature_sensor.getTempCByIndex(0);
        attempts++;
    }

    attempts = 0;

    return temperature;
}

void State::update()
{
    if (millis() - last_update < 1000)
        return;

    last_update = millis();

    previous_temperature = current_temperature;
    current_temperature = get_temperature();

    Serial.println("Current temperature: " + String(current_temperature));
    Serial.println("Previous temperature: " + String(previous_temperature));

    previous_resistance_state = resistance_state;
    resistance_state = digitalRead(RESISTANCE_PIN);

    previous_pump_state = pump_state;
    pump_state = digitalRead(PUMP_PIN);

    // Serial.println("Current temperature: " + String(current_temperature));
    // Serial.println("Previous temperature: " + String(previous_temperature));
    calculate_kws();
}

char *State::get_state_json()
{
    StaticJsonDocument<300> doc;

    doc["device_id"] = CUID;
    doc["current_temperature"] = isnan(current_temperature) ? -1 : current_temperature;
    doc["resistance_state"] = resistance_state;
    doc["pump_state"] = pump_state;
    doc["current_cycle"] = current_cycle;
    doc["current_term"] = current_term;
    doc["pump_power"] = pump_state ? PUMP_CONSUMPTION : 0;
    doc["resistance_power"] = resistance_state ? RESISTANCE_CONSUMPTION : 0;
    doc["pump_on_time_total"] = -1;
    doc["resistance_on_time_total"] = -1;
    doc["resistance_kwh"] = resistance_kwh;
    doc["pump_kwh"] = pump_kwh;

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
    doc["state"]["reported"]["current_cycle"] = current_cycle;
    doc["state"]["reported"]["current_term"] = current_term;
    doc["state"]["reported"]["current_term_duration"] = current_term_duration;
    doc["state"]["reported"]["current_cycle_duration"] = current_cycle_duration;
    doc["state"]["reported"]["pump_power"] = pump_state ? PUMP_CONSUMPTION : 0;
    doc["state"]["reported"]["resistance_power"] = resistance_state ? RESISTANCE_CONSUMPTION : 0;

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

        // We calculate the number of kilowatt-hours
        pump_kwh = (pump_on_time_interval * PUMP_CONSUMPTION) / (1000UL * 3600UL * 1000UL);
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

        // We calculate the number of kilowatt-hours
        resistance_kwh = (resistance_on_time_interval * RESISTANCE_CONSUMPTION) / (1000UL * 3600UL * 1000UL);
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
