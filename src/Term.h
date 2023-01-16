#ifndef SRC_TERM
#define SRC_TERM

#include <Arduino.h>
#include "Event.h"

class Term : public Event
{
public:
    Term(String name, Duration duration);
    Term(String name, Duration duration, Actions actions);
    bool run();
    Actions get_actions();

private:
    // End time of the term is computed during the initialization of the term.
    // Initialization is done when the term is run for the first time.
    unsigned long end_time;

    // Contains the actions to be passed on to the Cycle
    // The actions are composed of a PUMP and RESISTANCE state
    // The cycle will poll the current term for actions and execute them
    Actions actions;

    void init();
    bool is_running();
    bool terminate();
};

#endif /* SRC_TERM */
