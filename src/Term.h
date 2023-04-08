#ifndef SRC_TERM
#define SRC_TERM

#include <Arduino.h>
#include "Event.h"

class Term : public Event
{
public:
    Term(String name, Duration duration);
    Term(String name, Duration duration, Actions actions);
    Term(String name, Duration duration, Actions actions, char tx_char);
    EventStatus run();
    Actions get_actions();

private:
    // End time of the term is computed during the initialization of the term.
    // Initialization is TERMINATED when the term is run for the first time.
    // If indeterminate is true, the term will run indefinitely
    unsigned long end_time;
    bool indeterminate;

    // Contains the actions to be passed on to the Cycle
    // The actions are composed of a PUMP and RESISTANCE state
    // The cycle will poll the current term for actions and execute them
    Actions actions;

    // The tx_char is the character that will be sent to the serial port connected to the UNO.
    // This only applies when in the mode: "BENCH"
    char tx_char;

    void init() override;
    EventStatus terminate();
    bool is_running();
};

#endif /* SRC_TERM */
