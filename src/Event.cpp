#include "Event.h"
#include "Display.h"

Event::Event(String name, Duration duration)
    : name(name), duration(duration.toMillis()), status(IDLE), initialized(false), last_duration_update(0) {}

void Event::init()
{
    // Display event start information
    Display::update_state();

    initialized = true;
}

String Event::get_name()
{
    return name;
}

void Event::update_duration()
{
    if (millis() - last_duration_update < 1000)
        return;

    // Serial.print("Event duration: " + String(duration));

    duration -= 1000;

    last_duration_update = millis();
}

unsigned long Event::get_duration()
{
    return duration;
}

Buzzer Event::buzzer = Buzzer();
