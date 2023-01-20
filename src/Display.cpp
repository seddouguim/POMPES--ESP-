#include "Display.h"

Display::Display() : state(nullptr) {}

void Display::update(State *state)
{

    if (this->state == nullptr)
        this->state = state;

    update_resistance();
    update_pump();
    update_temperature();
}

void Display::update_resistance()
{
    if (!state->get_resistance_update())
        return;

    if (state->resistance_state == HIGH)
    {
        // Enables the animated fire picture
        Screen.print("page0.resTimer.en=1");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.resStatus.txt=\"ON\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.resStatus.pco=" + String(GREEN));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
    else
    {
        // Disables the animated fire picture
        Screen.print("page0.resTimer.en=0");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.fire.pic=" + String(RESISTANCE_OFF_PIC));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.resStatus.txt=\"OFF\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.resStatus.pco=" + String(RED));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
}

void Display::update_pump()
{
    if (!state->get_pump_update())
        return;

    if (state->pump_state == HIGH)
    {
        Screen.print("page0.pumpTimer.en=1");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.pumpStatus.txt=\"ON\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.pumpStatus.pco=" + String(GREEN));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
    else
    {
        Screen.print("page0.pumpTimer.en=0");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.pump.pic=" + String(PUMP_OFF_PIC));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.pumpStatus.txt=\"OFF\"");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("page0.pumpStatus.pco=" + String(RED));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);
    }
}

void Display::update_temperature()
{

    if (!state->get_temperature_update())
    {
        Screen.print("page0.tempDisplay.pco=" + String(GREEN));
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        Screen.print("vis page0.arrow,0");
        Screen.write(0xff);
        Screen.write(0xff);
        Screen.write(0xff);

        return;
    }

    // NEED TO UPDATE TEMPERATURE
    Screen.print("page0.tempDisplay.txt=\"" + String(state->current_temperature, 1) + char(176) + "\"");
    Screen.write(0xff);
    Screen.write(0xff);
    Screen.write(0xff);

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
