#ifndef SRC_EVENT
#define SRC_EVENT

#include <Arduino.h>
#include "utils.h"

class Event
{
public:
    Event(String name, Duration duration);
    String get_name();

private:
    String name;
    unsigned long duration;

    friend class Cycle;
    friend class Term;
};

#endif /* SRC_EVENT */
