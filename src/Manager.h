#ifndef SRC_MANAGER
#define SRC_MANAGER

#include <Arduino.h>
#include "State.h"
#include "Cycle.h"
#include "Network.h"
#include "Display.h"

class Manager
{
public:
    Manager(size_t num_cycles, Cycle *cycles);
    void run();

private:
    size_t num_cycles;
    Cycle *cycles;
    size_t current_cycle;

    State state;
    Network network;
    Display display;

    bool is_running();

    friend class Cycle;
};

#endif /* SRC_MANAGER */
