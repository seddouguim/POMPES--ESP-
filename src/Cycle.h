#ifndef SRC_CYCLE
#define SRC_CYCLE

#include "Event.h"
#include "Term.h"
#include "State.h"
#include "ActionHandler.h"

class Cycle : public Event
{
public:
    Cycle(String name, size_t num_terms, Term *terms);
    Cycle(String name, size_t num_terms, Term *terms, bool start_condition);

    EventStatus run(State *state);
    String get_term_name();
    unsigned long get_term_duration();

private:
    Term *terms;
    size_t num_terms;
    size_t current_term;

    // Save a reference to the state
    State *state;

    // The action handler is used to handle the actions of the current term
    // The action handler will be called by the cycle
    // Actions modify the status of the pump and the resistance
    ActionHandler handler;

    // Start condition is a boolean that is checked before the cycle starts
    // If the value is false, the cycle starts
    // We use this to start the cycle when the MAX_TEMPERATURE is reached
    // When the condition is met, start_conndition is set to false
    bool start_condition;
    EventStatus verify_start_condition();

    void init() override;
    EventStatus terminate();
    bool is_running();
    void update_state();
};

#endif /* SRC_CYCLE */
