#ifndef SRC_EVENT
#define SRC_EVENT

#include <Arduino.h>
#include "utils.h"

class Event
{
public:
    Event(String name, Duration duration);
    String get_name();
    unsigned long get_duration();
    unsigned long get_timer();
    unsigned long get_s_timer();

private:
    String name;
    unsigned long duration;
    unsigned long timer;
    unsigned long s_timer;

    EventStatus status;

    virtual void init();
    bool initialized;

    unsigned long last_duration_update;
    void update_duration();

    static Buzzer buzzer;

    friend class Cycle;
    friend class Term;
};

#endif /* SRC_EVENT */
