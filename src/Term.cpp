#include "Term.h"

Term::Term(String name, Duration duration)
    : Event(name, duration), end_time(0), indeterminate(false), tx_char('\n') {}

Term::Term(String name, Duration duration, Actions actions)
    : Event(name, duration), end_time(0), indeterminate(false), actions(actions), tx_char('\n') {}

Term::Term(String name, Duration duration, Actions actions, char tx_char)
    : Event(name, duration), end_time(0), indeterminate(false), actions(actions), tx_char(tx_char) {}

void Term::init()
{
    // Play a beep to indicate that the term starts
    buzzer.play(TERM_BEEP_AMOUNT);

    // check if duration is set to -1
    // if so, the term will run indefinitely
    if (duration < 0)
    {
        indeterminate = true;
        return;
    }

    // We compute the term's end time
    end_time = millis() + duration;

    // we call @super init method
    Event::init();
}

bool Term::is_running()
{
    // If the term is indeterminate, it will run indefinitely
    if (indeterminate)
        return true;

    // If the current time is less than the end time, the term is still running
    return millis() < end_time;
}

void Term::terminate()
{
    // We re-initialize the term
    end_time = 0;

    // If we're in the "BENCH" mode, we send tx_char to the UNO
    if (PROGRAM_MODE == BENCH)
        UNO_TX.print("<" + String(tx_char) + ">");

    // Play a beep to indicate that the term ends
    buzzer.play(TERM_BEEP_AMOUNT);
}

Actions Term::get_actions()
{
    return actions;
}

EventStatus Term::run()
{
    // if the end time is 0, it means that the term has not been initialized
    // Therefore, we initialize it
    if (!end_time)
        init();

    // if the term is not running, we return false
    // This means that the term is finished
    // The cycle will then move to the next term
    if (!is_running())
    {
        terminate();
        return TERMINATED;
    }

    // We return true because the term is still running
    return RUNNING;
}
