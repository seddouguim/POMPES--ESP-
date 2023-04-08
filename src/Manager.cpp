#include "Manager.h"

Manager::Manager(size_t num_cycles, Cycle *cycles)
    : num_cycles(num_cycles), cycles(cycles),
      current_cycle(0), status(IDLE), state(State()), network(Network()), initialized(false) {}

bool Manager::is_running()
{
    return current_cycle < num_cycles;
}

EventStatus Manager::get_status()
{
    return status;
}

void Manager::init()
{
    if (initialized)
        return;

    // we bind this current manager to the display
    // This is used to access the manager from the display
    Display::manager = this;
    initialized = true;
}

void Manager::run()
{
    init();

    // At each iteration, we update the state
    state.update();

    // We run the display loop
    // This loop is in charge of updating the display
    // It is also in charge of listening to messages from the display
    display.loop();

    // We run the network loop
    // We also send a reference to the state
    network.loop(&state);

    // If all the cycles are finished, we turn off the pump and the resistance
    if (!this->is_running())
    {
        // if the resistance and the pump are running, we turn them off.
        if (digitalRead(PUMP_PIN) == HIGH || digitalRead(RESISTANCE_PIN) == HIGH)
        {
            digitalWrite(PUMP_PIN, LOW);
            digitalWrite(RESISTANCE_PIN, LOW);

            // We update the display
            Display::update_state();
        }

        return;
    }

    // if the "program" is still running, we run the current cycle
    // We also send a reference to the state
    // If the cycle is finished, we move to the next cycle
    status = cycles[current_cycle].run(&state);
    if (status == EventStatus::TERMINATED && this->is_running())
        current_cycle++;
}