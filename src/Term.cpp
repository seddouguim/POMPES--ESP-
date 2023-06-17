#include "Term.h"
#include "Display.h"

Term::Term(String name, Duration duration)
    : Event(name, duration), end_time(0), indeterminate(false), tx_char('\n') {}

Term::Term(String name, Duration duration, Actions actions)
    : Event(name, duration), end_time(0), indeterminate(false), actions(actions), tx_char('\n') {}

Term::Term(String name, Duration duration, Actions actions, char tx_char)
    : Event(name, duration), end_time(0), indeterminate(false), actions(actions), tx_char(tx_char) {}

void Term::init()
{
    // we call @super init method
    Event::init();

    // Play a beep to indicate that the term starts
    buzzer.play(TERM_BEEP_AMOUNT);
    // Serial.println("Buzzer from Term_init");

    // check if duration is set to -1
    // if so, the term will run indefinitely
    if (duration < 0)
    {
        indeterminate = true;
        return;
    }

    // We compute the term's end time
    end_time = millis() + duration;

    // We set up the timers
    timer = DEBUG ? duration * DEBUG_RATIO : duration;
    s_timer = duration;

    sent_tx_char = false;
}

bool Term::is_running()
{
    // If the term is indeterminate, it will run indefinitely
    if (indeterminate)
        return true;

    // If the current time is less than the end time, the term is still running
    return millis() < end_time;
}

EventStatus Term::terminate()
{
    // We re-initialize the term
    end_time = 0;

    // If we're in the "BENCH" mode, we send tx_char to the UNO
    if (PROGRAM_MODE == BENCH && tx_char != '\n' && !sent_tx_char)
        UNO_TX.print("<" + String(tx_char) + ">");

    // Play a beep to indicate that the term ends
    // buzzer.play(TERM_BEEP_AMOUNT);

    sent_tx_char = true;

    return TERMINATED;
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

    update_duration();

    // if the term is not running, we return false
    // This means that the term is finished
    // The cycle will then move to the next term
    if (!is_running())
        return terminate();

    // We return true because the term is still running
    return RUNNING;
}
