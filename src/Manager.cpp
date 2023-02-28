#include "Manager.h"

#define PAUSE false

Manager::Manager(size_t num_cycles, Cycle *cycles)
    : num_cycles(num_cycles), cycles(cycles),
      current_cycle(0), state(State()), network(Network()) {}

bool Manager::is_running()
{
    return current_cycle < num_cycles;
}

void Manager::run()
{
    // if the manager is not running, we return
    if (!is_running())
        return;

    // At each iteration, we update the state
    state.update();

    // We update the display
    display.update(&state);

    // Run network loop
    network.loop(&state);

    // We run the current cycle
    // We also send a reference to the state
    // If the cycle is finished, we move to the next cycle
    if (PAUSE)
        return;

    if (cycles[current_cycle].run(&state) == 0)
    {
        Serial.println("Cycle finished");
        current_cycle++;
    }
}