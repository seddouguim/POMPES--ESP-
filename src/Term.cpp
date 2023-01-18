#include "Term.h"

Term::Term(String name, Duration duration)
    : Event(name, duration), end_time(0) {}

Term::Term(String name, Duration duration, Actions actions)
    : Event(name, duration), end_time(0), actions(actions) {}

void Term::init()
{
    // We compute the term's end time
    end_time = millis() + duration;

    Serial.println(get_name() + " started. (duration: " + String(duration / 1000) + " seconds)");
}

bool Term::is_running()
{
    // We check if the current time is before the term's end time
    return millis() < end_time;
}

bool Term::terminate()
{
    // We re-initialize the term
    end_time = 0;

    // Play a beep to indicate that the term is finished
    buzzer.play(TERM_BEEP_AMOUNT);
    Serial.println(get_name() + " finished.");

    return false;
}

Actions Term::get_actions()
{
    return actions;
}

bool Term::run()
{
    // if the end time is 0, it means that the term has not been initialized
    // Therefore, we initialize it
    if (!end_time)
        init();

    // if the term is not running, we return false
    // This means that the term is finished
    // The cycle will then move to the next term
    if (!is_running())
        return terminate();

    // We return true because the term is still running
    return true;
}
