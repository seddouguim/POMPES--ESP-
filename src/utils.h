#ifndef SRC_UTILS
#define SRC_UTILS

#include <Arduino.h>
#include "config.h"

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

        // Serial.println("Initialized buzzer.");

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

struct Message
{
    String text;
    int duration; // in seconds

    unsigned long getDuration()
    {
        return duration * 1000;
    }
} typedef Message;

enum Command
{
    GET = 1,
    PAG,
    CON,
    DIS,
    UNKNOWN_COMMAND
};

enum Page
{
    main = 1,
    WIFI_LIST,
    WIFI_CREDS,
    SETTINGS,
    UNKNOWN_PAGE
};

enum Mode
{
    BENCH,
    OFFICIAL,
    USER
};

enum EventStatus
{
    IDLE,
    RUNNING,
    TERMINATED,
};

Page get_page(String page);
Command get_command(String command);

#endif /* SRC_UTILS */
