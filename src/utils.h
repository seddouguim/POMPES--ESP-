#ifndef SRC_UTILS
#define SRC_UTILS

#include <Arduino.h>
#include "config.h"

// GIT REMOTE TEST

#define OFF 0
#define ON 1
#define AUTO 2
#define TOGGLE 3
#define NIL -7

typedef struct Duration
{
    int hours;
    int minutes;
    int seconds;

    Duration(int hours = 0, int minutes = 0, int seconds = 0)
        : hours(hours), minutes(minutes), seconds(seconds){};

    unsigned long toMillis()
    {
        unsigned long millis = (hours * 3600 + minutes * 60 + seconds) * 1000;
        return DEBUG ? millis / DEBUG_RATIO : millis;
    }

} Duration;

typedef struct Actions
{
    int pump;
    int resistance;
} Actions;

typedef struct Buzzer
{
    byte initialized;

    int frequency;
    int duration;

    Buzzer() : initialized(0){};

    void init()
    {
        if (initialized)
            return;

        pinMode(BUZZER_PIN, OUTPUT);

        frequency = 1000;
        duration = 100;

        initialized = 1;
    }

    void play(int times)
    {
        init();

        for (int i = 0; i < times; i++)
        {
            tone(BUZZER_PIN, frequency, duration);
            delay(duration);
        }
    }
} Buzzer;

#endif /* SRC_UTILS */
