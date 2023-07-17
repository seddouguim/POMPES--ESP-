#include "Manager.h"

Manager::Manager(size_t num_cycles, Cycle *cycles)
    : num_cycles(num_cycles), cycles(cycles),
      current_cycle(-1), status(IDLE), state(State()), network(Network()), initialized(false) {}

bool Manager::is_running()
{
    if (current_cycle < 0)
        return false;

    return current_cycle < num_cycles;
}

EventStatus Manager::get_status()
{
    return status;
}

String Manager::get_current_cycle()
{
    return state.current_cycle;
}

String Manager::get_current_term()
{
    return state.current_term;
}

void Manager::init()
{
    if (initialized)
        return;

    // CYCLES: IDLING, DRAINING, V_40
    current_cycle = START_CYCLE;

    Display::manager = this;
    initialized = true;
}

void Manager::run()
{
    init();

    // At each iteration, we update the state
    state.update();

    // We run the display loop
    display.loop();

    // We run the network loop
    network.loop(&state);

    if (!this->is_running())
    {

        if (digitalRead(PUMP_PIN) == HIGH || digitalRead(RESISTANCE_PIN) == HIGH)
        {
            digitalWrite(PUMP_PIN, LOW);
            digitalWrite(RESISTANCE_PIN, LOW);
        }

        if (this->state.current_cycle == "TERMINATED")
            return;

        this->state.current_cycle = "TERMINATED";
        this->state.current_term = "N/A";

        this->state.current_cycle_duration = 0;
        this->state.current_term_duration = 0;

        current_cycle = -1;

        Display::update_state();

        return;
    }

    status = cycles[current_cycle].run(&state);
    if (status == EventStatus::TERMINATED && this->is_running())
        current_cycle++;
}