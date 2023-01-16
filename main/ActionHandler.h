#ifndef SRC_ACTIONHANDLER
#define SRC_ACTIONHANDLER

#include <Arduino.h>
#include "utils.h"
#include "State.h"
#include "config.h"

class ActionHandler
{
public:
    ActionHandler();
    void execute_actions(Actions actions, State *state);
    void handle_pump();
    void handle_resistance();

private:
    Actions actions;
    State *state;
    unsigned long toggle_pump_start_time;
};

#endif /* SRC_ACTIONHANDLER */
