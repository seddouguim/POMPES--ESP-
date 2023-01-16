#ifndef SRC_UTILS
#define SRC_UTILS

#define DEBUG false
#define DEBUG_RATIO 360

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

#endif /* SRC_UTILS */
