#include "ActionHandler.h"

ActionHandler::ActionHandler()
    : actions({NIL, NIL}), state(nullptr), toggle_pump_start_time(0ul) {}

void ActionHandler::execute_actions(Actions actions, State *state)
{
    // Check if state is nullptr
    // If it is, then we save a reference to the state
    if (this->state == nullptr)
        this->state = state;

    // if the given actions are different from the ones saved, then we update the saved ones
    if (this->actions.pump != actions.pump || this->actions.resistance != actions.resistance)
        this->actions = actions;

    // handle the pump and the resistance
    handle_pump();
    handle_resistance();
}

void ActionHandler::handle_resistance()
{
    if (actions.resistance == state->resistance_state)
        return;

    switch (actions.resistance)
    {
    case ON:
        digitalWrite(RESISTANCE_PIN, HIGH);
        Serial.println("Resistance ON.");
        break;

    case OFF:
        digitalWrite(RESISTANCE_PIN, LOW);
        Serial.println("Resistance OFF.");
        break;

    case AUTO:
        if (state->current_temperature >= MAX_TEMPERATURE && state->resistance_state == ON)
        {
            digitalWrite(RESISTANCE_PIN, LOW);
            Serial.println("Temperature " + String(state->current_temperature) + " > " + String(MAX_TEMPERATURE) + " -> Resistance OFF.");
        }
        else if (state->current_temperature <= MIN_TEMPERATURE && state->resistance_state == OFF)
        {
            digitalWrite(RESISTANCE_PIN, HIGH);
            Serial.println("Temperature " + String(state->current_temperature) + " < " + String(MIN_TEMPERATURE) + " -> Resistance ON.");
        }
        break;
    }
}

void ActionHandler::handle_pump()
{
    if (actions.pump == state->pump_state)
        return;

    switch (actions.pump)
    {
    case ON:
        digitalWrite(PUMP_PIN, HIGH);
        Serial.println("Pump ON.");
        break;

    case OFF:
        digitalWrite(PUMP_PIN, LOW);
        Serial.println("Pump OFF.");
        break;

    // turn pump on for 20 seconds and turn off for 40 seconds
    case TOGGLE:
        if (toggle_pump_start_time == 0ul)
            toggle_pump_start_time = millis();

        if (state->pump_state == OFF && millis() == toggle_pump_start_time)
        {
            digitalWrite(PUMP_PIN, HIGH);
            Serial.println("Pump ON for 20 seconds.");
        }
        if (millis() - toggle_pump_start_time > 20000 && state->pump_state == ON)
        {
            digitalWrite(PUMP_PIN, LOW);
            Serial.println("Pump OFF for 40 seconds.");
        }

        if (millis() - toggle_pump_start_time > 60000)
            toggle_pump_start_time = 0ul;
        break;

    case AUTO:
        if (state->current_temperature >= MAX_TEMPERATURE && state->pump_state == ON)
        {
            digitalWrite(PUMP_PIN, LOW);
            Serial.println("Temperature " + String(state->current_temperature) + " > " + String(MAX_TEMPERATURE) + " -> Pump OFF.");
        }
        else if (state->current_temperature <= MIN_TEMPERATURE && state->pump_state == OFF)
        {
            digitalWrite(PUMP_PIN, HIGH);
            Serial.println("Temperature " + String(state->current_temperature) + " < " + String(MIN_TEMPERATURE) + " -> Pump ON.");
        }
        break;
    }
}