#ifndef SRC_DISPLAY
#define SRC_DISPLAY

#include "State.h"

class Display
{
public:
    Display();
    void update(State *state);

private:
    // Reference to State
    State *state;

    void update_resistance();
    void update_pump();
    void update_temperature();
};

#endif /* SRC_DISPLAY */
