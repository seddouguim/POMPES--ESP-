#include "Cycle.h"
#include "utils.h"
#include "Display.h"

Cycle::Cycle(String name, size_t num_terms, Term *terms)
    : Event(name, Duration(0)), terms(terms), num_terms(num_terms), current_term(START_TERM),
      state(nullptr), handler(ActionHandler()), start_condition(false) {}

Cycle::Cycle(String name, size_t num_terms, Term *terms, bool start_condition)
    : Event(name, Duration(0)), terms(terms), num_terms(num_terms), current_term(START_TERM),
      state(nullptr), handler(ActionHandler()), start_condition(start_condition) {}

bool Cycle::is_running()
{
    return current_term < num_terms;
}

void Cycle::init()
{
    //  Compute the duration of the cycle
    //  Duration is the sum of the durations of the terms
    for (size_t i = START_TERM; i < num_terms; i++)
    {
        duration += terms[i].duration;
    }

    // We set up the timers
    timer = DEBUG ? duration * DEBUG_RATIO : duration;
    s_timer = duration;

    // Play a beep to indicate that the cycle starts
    buzzer.play(CYCLE_BEEP_AMOUNT);
}

EventStatus Cycle::terminate()
{

    current_term = 0;
    duration = 0ul;

    // we play a beep to indicate that the cycle is finished
    buzzer.play(CYCLE_BEEP_AMOUNT);

    if (name == "V40")
        Display::update_state();

    return TERMINATED;
}

String Cycle::get_term_name()
{
    return terms[current_term].get_name();
}

unsigned long Cycle::get_term_timer()
{
    return terms[current_term].get_timer();
}

unsigned long Cycle::get_term_s_timer()
{
    return terms[current_term].get_s_timer();
}

EventStatus Cycle::verify_start_condition()
{
    // If the resistance is not already on, we turn it on
    // start_condition => HEATING UP (resistance on)
    if (state->resistance_state == LOW)
        digitalWrite(RESISTANCE_PIN, HIGH);

    // if temperature is higher than MAX_TEMPERATURE
    // we set the start_condition to false (condition is met)
    // and we return 1 to indicate that the cycle can start
    if (state->current_temperature >= MAX_TEMPERATURE)
    {
        start_condition = false;
        return RUNNING;
    }

    // We return EventStatus::IDLE to indicate that the start condition is not met yet
    return IDLE;
}

void Cycle::update_state()
{
    if (start_condition)
    {
        this->state->current_cycle = "HEATING UP";
        return;
    }

    this->state->current_cycle = this->get_name();
    this->state->current_term = this->get_term_name();

    this->state->current_cycle_duration = this->get_timer();
    this->state->current_term_duration = this->get_term_timer();
}

EventStatus Cycle::run(State *state)
{
    // If state is nullptr, we save a reference to the State
    if (this->state == nullptr)
        this->state = state;

    // We update the current_term and current_cycle in the state object.
    update_state();

    // We check if start condition is met
    // If not, we return -1
    if (start_condition)
        return verify_start_condition();

    // if the duration is 0, it means that the cycle has not been initialized => we initialize it
    if (!duration)
        init();

    // if the cycle is not running, we return false
    // This means that the cycle is finished
    // The manager will then move to the next cycle
    if (!is_running())
        return terminate();

    // We run the current term and save the status
    status = terms[current_term].run();

    update_duration();

    // If the term is finished, we move to the next term
    if (status == TERMINATED)
        current_term++;

    // We call the ActionHandler to execute the actions of the current term
    handler.execute_actions(terms[current_term].get_actions(), state);

    // we return true to indicate that the cycle is still running
    return RUNNING;
}