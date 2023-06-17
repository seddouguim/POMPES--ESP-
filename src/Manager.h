#ifndef SRC_MANAGER
#define SRC_MANAGER

#include <Arduino.h>
#include "Cycle.h"
#include "Network.h"
#include "Display.h"
#include "State.h"

class Manager
{
public:
    Manager(size_t num_cycles, Cycle *cycles);
    void run();
    EventStatus get_status();

    String get_current_cycle();
    String get_current_term();

private:
    size_t num_cycles;
    Cycle *cycles;
    size_t current_cycle;
    EventStatus status;

    State state;
    Network network;
    Display display;

    bool is_running();
    void init();
    bool initialized;

    friend class Cycle;
    friend class Display;
};

#endif /* SRC_MANAGER */
