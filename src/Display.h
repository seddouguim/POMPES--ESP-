#ifndef SRC_DISPLAY
#define SRC_DISPLAY

#include "State.h"

class Display
{
public:
    Display();
    void update(State *state);
    void terminate();
    static void update_cycle(String cycle, String term, int color);

private:
    // Reference to State
    State *state;

    unsigned long last_update;
    bool initialized;

    String command;

    void init();

    void update_resistance();
    void update_pump();
    void update_temperature();
};

#endif /* SRC_DISPLAY */
