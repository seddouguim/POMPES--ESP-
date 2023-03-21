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
    // At each iteration, we update the state
    state.update();

    // We update the display
    display.update(&state);

    // Run network loop
    network.loop(&state);

    // If all the cycles are finished, we return
    if (!is_running())
    {
        display.terminate();

        // if the resistance and the pump are running, we turn them off.
        if (digitalRead(PUMP_PIN) == HIGH)
            digitalWrite(PUMP_PIN, LOW);

        if (digitalRead(RESISTANCE_PIN) == HIGH)
            digitalWrite(RESISTANCE_PIN, LOW);

        Serial.println("All cycles finished.");

        return;
    }

    if (PAUSE)
        return;

    // We run the current cycle
    // We also send a reference to the state
    // If the cycle is finished, we move to the next cycle
    if (cycles[current_cycle].run(&state, display.update_cycle) == 0)
    {
        current_cycle++;
    }
}