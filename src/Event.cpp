#include "Event.h"
#include "Display.h"

Event::Event(String name, Duration duration)
    : name(name), duration(duration.toMillis()), status(IDLE), initialized(false) {}

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

unsigned long Event::get_duration()
{
    return duration;
}

Buzzer Event::buzzer = Buzzer();
