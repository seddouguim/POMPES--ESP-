#include <Arduino.h>
#include "Manager.h"
#include "Cycle.h"
#include "Term.h"
#include "LittleFS.h"
#include "utils.h"

// Cycles used by "BENCH" and "OFFICIAL" modes
Term IDLING_TERMS[] = {
    Term("Term 1", Duration{1, 30, 0}, Actions{.pump = TOGGLE, .resistance = AUTO}, 'A'),
    Term("Term 2", Duration{10, 35, 0}, Actions{.pump = OFF, .resistance = OFF}),
    Term("Term 3", Duration{1, 5, 0}, Actions{.pump = ON, .resistance = ON}, 'A'), // We send an 'A' to the UNO
};

Term DRAINING_TERMS[] = {
    Term("Term 1", Duration{15, 30, 34}, Actions{.pump = AUTO, .resistance = AUTO}),
    Term("Term 2", Duration{8, 29, 26}, Actions{.pump = OFF, .resistance = OFF}),
};

Term V_40_TERMS[] = {
    Term("Term 1", Duration{11, 35, 0}, Actions{.pump = AUTO, .resistance = AUTO}),
    Term("Term 2", Duration{0, 30, 0}, Actions{.pump = ON, .resistance = ON}),
    Term("Term 3", Duration{0, 45, 0}, Actions{.pump = OFF, .resistance = OFF}, 'B'), // We send a 'B' to the UNO
};

Cycle cycles[] = {
    Cycle("IDLING", 3, IDLING_TERMS, true), // We set the start condition to true (WARMING UP)
    Cycle("DRAINING", 2, DRAINING_TERMS),
    Cycle("V40", 3, V_40_TERMS),
};

//----------------------------------------------------------
// Cycles and Terms used by "USER" mode
Term USER_DRAINING_TERMS[] = {
    Term("AUTO", Duration{-1}, Actions{.pump = AUTO, .resistance = AUTO}),
};

Cycle user_cycles[] = {
    Cycle("DRAINING", 1, USER_DRAINING_TERMS, true), // We set the start condition to true (WARMING UP)
};

Manager CycleManager =
    PROGRAM_MODE == USER ? Manager(1, user_cycles) : Manager(3, cycles);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial1.begin(115200);
    delay(1000);

    // test serial 1
    Serial1.print("<ALLLL>");
    delay(1000);

    LittleFS.begin();
    delay(1000);
}

void loop()
{
    CycleManager.run();
    ;
}