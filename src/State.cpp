#include "State.h"

State::State()
    : Thermocouple(Adafruit_MAX31855(CLK, CS, MISO))
{
    init();
}

void State::init()
{
    Serial.println("Initializing thermocouple...");
    Thermocouple.begin();
    delay(500);
    Serial.println("Thermocouple initialized.");

    current_temperature = 0;
    previous_temperature = 0;

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

    pinMode(RESISTANCE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);

    // Make sure the pump is OFF and the resistance is ON
    digitalWrite(RESISTANCE_PIN, HIGH);
    digitalWrite(PUMP_PIN, LOW);
}

void State::update()
{
    previous_temperature = current_temperature;
    current_temperature = Thermocouple.readCelsius();
    // current_temperature += 5;

    previous_resistance_state = resistance_state;
    resistance_state = digitalRead(RESISTANCE_PIN);

    previous_pump_state = pump_state;
    pump_state = digitalRead(PUMP_PIN);

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
    {
        pump_start_timer = millis();

        Serial.println("Pump state just turned on, calculating kws.");
        Serial.println("Calculations start time: " + String(pump_start_timer));
    }

    // Pump has just turned OFF
    // We calculate the time the PUMP has been on
    else
    {

        unsigned long pump_on_time_interval = millis() - pump_start_timer;
        pump_on_time_total += pump_on_time_interval;

        Serial.println("Pump state just changed to: OFF, calculating kws.");
        Serial.println("End time: " + String(millis()));

        Serial.println("ON interval:  " + String(pump_on_time_interval));
        Serial.println("Pump total ON time:  " + String(pump_on_time_total));

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
    {
        resistance_start_timer = millis();

        Serial.println("Resistance state just changed to: ON, starting timer.");
        Serial.println("Start time: " + String(resistance_start_timer));
    }

    // Resistance has just turned OFF
    // We calculate the time the resistance has been on
    else
    {
        unsigned long resistance_on_time_interval = millis() - resistance_start_timer;
        resistance_on_time_total += resistance_on_time_interval;

        Serial.println("Resistance state just changed to: OFF, calculating kws.");
        Serial.println("End time: " + String(millis()));

        Serial.println("ON interval:  " + String(resistance_on_time_interval));
        Serial.println("Resistance total ON time:  " + String(resistance_on_time_total));

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
