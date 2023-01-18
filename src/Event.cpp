#include "Event.h"

Event::Event(String name, Duration duration)
    : name(name), duration(duration.toMillis())
{
}

String Event::get_name()
{
    return name;
}

Buzzer Event::buzzer = Buzzer();
