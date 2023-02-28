#include "Display.h"

Display::Display() : state(nullptr), last_update(0ul), command("") {}

void Display::update(State *state)
{

    if (this->state == nullptr)
        this->state = state;

    if (millis() - last_update < 1000)
        return;

    last_update = millis();

    update_resistance();
    update_pump();
    update_temperature();

    Serial.println("Sending updates to display...");
}

void Display::update_resistance()
{
    if (!state->get_resistance_update())
    {
        Serial.println("Resistance update not needed.");
    }

    if (state->resistance_state == HIGH)
    {
        Serial.println("Updating resistance state to ON.");

        command = "page0.resTimer.en=1";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.resStatus.txt=\"ON\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.resStatus.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);
    }
    else
    {
        Serial.println("Updating resistance state to OFF.");

        command = "page0.resTimer.en=0";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.res.pic=" + String(RESISTANCE_OFF_PIC);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.resStatus.txt=\"OFF\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.resStatus.pco=" + String(RED);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);
    }
}

void Display::update_pump()
{
    if (!state->get_pump_update())
    {
        Serial.println("Pump update not needed.");
        return;
    }

    if (state->pump_state == HIGH)
    {
        Serial.println("Updating pump state to ON.");

        command = "page0.pumpTimer.en=1";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.txt=\"ON\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);
    }
    else
    {
        Serial.println("Updating pump state to OFF.");

        command = "page0.pumpTimer.en=0";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.pump.pic=" + String(PUMP_OFF_PIC);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.txt=\"OFF\"";
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        command = "page0.pumpStatus.pco=" + String(RED);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);
    }
}

void Display::update_temperature()
{

    if (!state->get_temperature_update())
    {
        Serial.println("Temperature update not needed.");
        Serial.println("Changing temperature display to green.");
        command = "page0.tempDisplay.pco=" + String(GREEN);
        Screen.print(command);
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
        Serial.println("Command sent: " + command);

        Screen.print("vis page0.arrow,0");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        return;
    }

    // NEED TO UPDATE TEMPERATURE

    Serial.println("Updating temperature display...");

    command = "page0.tempDisplay.txt=\"" + String(state->current_temperature, 1) + char(176) + "\"";
    Screen.print(command);
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);
    Serial.println("Command sent: " + command);

    float temperatureTrend = state->current_temperature - state->previous_temperature;

    if (temperatureTrend > 0)
    {
        Screen.print("page0.tempDisplay.pco=" + String(RED));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.arrow.pic=9");
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

        Screen.print("page0.arrow.pic=8");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("vis page0.arrow,1");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
}
