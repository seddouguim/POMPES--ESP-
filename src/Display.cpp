#include "Display.h"

Display::Display() : state(nullptr), last_update(0ul), initialized(false), command("") {}

void Display::update(State *state)
{

    if (this->state == nullptr)
        this->state = state;

    if (!initialized)
    {
        init();
        return;
    }

    if (millis() - last_update < 1000)
        return;

    last_update = millis();

    SERIAL_DEBUG &&
        Serial.println("Sending updates to display...");

    update_resistance();
    update_pump();
    update_temperature();
}

void Display::init()
{
    if (initialized)
        return;

    SERIAL_DEBUG &&
        Serial.println("Initializing display...");

    command = "page0.pumpStatus.pco=" + String(RED);
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.pumpStatus.txt=\"OFF\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.pumpTimer.en=0";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.pump.pic=" + String(PUMP_OFF_PIC);
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.resStatus.txt=\"OFF\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.resStatus.pco=" + String(RED);
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.resTimer.en=0";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.fire.pic=" + String(RESISTANCE_OFF_PIC);
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.state.txt=\"INIT...\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    command = "page0.state.pco=" + String(ORANGE);
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    delay(100);

    command = "page0.tempDisplay.txt=\"--.-\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    delay(100);

    Screen.print("vis page0.arrow,0");
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    initialized = true;

    SERIAL_DEBUG &&
        Serial.println("Display initialized.");
}

void Display::update_resistance()
{
    if (!state->get_resistance_update())
    {
        SERIAL_DEBUG &&
            Serial.println("Resistance update not needed.");

        return;
    }

    if (state->resistance_state == HIGH)
    {
        command = "page0.resTimer.en=1";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.resStatus.txt=\"ON\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.resStatus.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);
    }
    else
    {
        SERIAL_DEBUG &&
            Serial.println("Updating resistance state to OFF.");

        command = "page0.resTimer.en=0";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.fire.pic=" + String(RESISTANCE_OFF_PIC);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.resStatus.txt=\"OFF\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.resStatus.pco=" + String(RED);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);
    }
}

void Display::update_pump()
{
    if (!state->get_pump_update())
    {
        SERIAL_DEBUG &&
            Serial.println("Pump update not needed.");

        return;
    }

    if (state->pump_state == HIGH)
    {
        SERIAL_DEBUG &&
            Serial.println("Updating pump state to ON.");

        command = "page0.pumpTimer.en=1";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.txt=\"ON\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);
    }
    else
    {

        SERIAL_DEBUG &&
            Serial.println("Updating pump state to OFF.");

        command = "page0.pumpTimer.en=0";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.pump.pic=" + String(PUMP_OFF_PIC);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.txt=\"OFF\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.pco=" + String(RED);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);
    }
}

void Display::update_temperature()
{

    if (!state->get_temperature_update())
    {
        SERIAL_DEBUG &&
            Serial.println("Temperature update not needed.");

        command = "page0.tempDisplay.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        SERIAL_DEBUG &&
            Serial.println("Command sent: " + command);

        Screen.print("vis page0.arrow,0");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        return;
    }

    // NEED TO UPDATE TEMPERATURE

    SERIAL_DEBUG &&
        Serial.println("Updating temperature display...");

    command = "page0.tempDisplay.txt=\"" + String(state->current_temperature, 1) + char(176) + "\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    SERIAL_DEBUG &&
        Serial.println("Command sent: " + command);

    float temperatureTrend = state->current_temperature - state->previous_temperature;

    if (temperatureTrend > 0)
    {
        Screen.print("page0.tempDisplay.pco=" + String(RED));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.arrow.pic=" + String(ARROW_UP_PIC));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("vis page0.arrow,1");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        return;
    }

    else if (temperatureTrend < 0)
    {
        Screen.print("page0.tempDisplay.pco=" + String(BLUE));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.arrow.pic=" + String(ARROW_DOWN_PIC));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("vis page0.arrow,1");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
}

void Display::update_cycle(String cycle, String term, int color)
{
    if (term == "")
    {
        Screen.print("page0.state.txt=\"" + cycle + "\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }

    else
    {
        Screen.print("page0.state.txt=\"" + cycle + " - " + term + "\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }

    Screen.print("page0.state.pco=" + String(color));
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);
}

void Display::terminate()
{
    Screen.print("page0.state.txt=\"DONE!\"");
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

    Screen.print("page0.state.pco=" + String(LIGHTGREEN));
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);
}