#include "Cycle.h"

Cycle::Cycle(String name, size_t num_terms, Term *terms)
    : Event(name, Duration(0)), terms(terms), num_terms(num_terms), current_term(0),
      state(nullptr), handler(ActionHandler()), start_condition(false) {}

Cycle::Cycle(String name, size_t num_terms, Term *terms, bool start_condition)
    : Event(name, Duration(0)), terms(terms), num_terms(num_terms), current_term(0),
      state(nullptr), handler(ActionHandler()), start_condition(start_condition) {}

bool Cycle::is_running()
{
    return current_term < num_terms;
}

void Cycle::init()
{
    // Compute the duration of the cycle
    // Duration is the sum of the durations of the terms
    for (size_t i = 0; i < num_terms; i++)
    {
        duration += terms[i].duration;
    }

    Serial.println(get_name() + " started.");
}

int Cycle::terminate()
{
    // We re-initialize the cycle
    current_term = 0;
    duration = 0ul;

    Serial.println(get_name() + " finished.");
    return 0;
}

int Cycle::verify_start_condition()
{
    // if temperature is higher than MAX_TEMPERATURE
    // we set the start_condition to false
    // i.e the cycle starts
    if (state->current_temperature >= MAX_TEMPERATURE)
    {
        start_condition = false;
        Serial.println("Start condition met.");
        Serial.println("Starting " + get_name() + ".");

        return 1;
    }

    return -1;
}

int Cycle::run(State *state)
{
    // If state is nullptr, we save a reference to the State
    if (this->state == nullptr)
        this->state = state;

    // We check if start condition is met
    // If not, we return -1
    if (start_condition)
        return verify_start_condition();

    // if the duration is 0, it means that the cycle has not been initialized
    // Therefore, we initialize it
    if (!duration)
        init();

    // if the cycle is not running, we return false
    // This means that the cycle is finished
    // The manager will then move to the next cycle
    if (!is_running())
        return terminate();

    // We run the current term
    // If the term is finished, we move to the next term
    if (!terms[current_term].run())
        current_term++;

    // We call the ActionHandler to execute the actions of the current term
    handler.execute_actions(terms[current_term].get_actions(), state);

    // We return true because the cycle is still running
    return 1;
}