#include "Event.h"
#include "Display.h"

Event::Event(String name, Duration duration)
    : name(name), duration(duration.toMillis()), status(UNKNOWN_STATUS), initialized(false) {}

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

Buzzer Event::buzzer = Buzzer();
