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
    return this->cycles[current_cycle].get_name();
}

String Manager::get_current_term()
{
    return this->cycles[current_cycle].get_term_name();
}

unsigned long Manager::get_current_cycle_duration()
{
    return cycles[current_cycle].get_duration();
}

unsigned long Manager::get_current_term_duration()
{
    return cycles[current_cycle].get_term_duration();
}

void Manager::init()
{
    if (initialized)
        return;

    current_cycle = 0;

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