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

    // Display Related
    void init();
    void update_temperature();
    void update_resistance();
    void update_pump();
};

#endif /* SRC_DISPLAY */
