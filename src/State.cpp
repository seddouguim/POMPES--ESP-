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

    pinMode(RESISTANCE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);

    // Make sure the pump is OFF and the resistance is ON
    digitalWrite(RESISTANCE_PIN, HIGH);
    digitalWrite(PUMP_PIN, LOW);
}

void State::update()
{
    previous_temperature = current_temperature;
    // current_temperature = Thermocouple.readCelsius();
    current_temperature += 5;

    previous_resistance_state = resistance_state;
    resistance_state = digitalRead(RESISTANCE_PIN);

    previous_pump_state = pump_state;
    pump_state = digitalRead(PUMP_PIN);
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
