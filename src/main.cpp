#include <Arduino.h>
#include <Arduino.h>

#include "Manager.h"
#include "Cycle.h"
#include "Term.h"
#include "utils.h"

// We define the terms
Term IDLING_TERMS[] = {
    Term("Term 1", Duration{1, 30, 0}, Actions{.pump = TOGGLE, .resistance = AUTO}),
    Term("Term 2", Duration{10, 35, 0}, Actions{.pump = OFF, .resistance = OFF}),
    Term("Term 2", Duration{1, 5, 0}, Actions{.pump = ON, .resistance = ON}),
};

Term DRAINING_TERMS[] = {
    Term("term 1", Duration{15, 30, 34}, Actions{.pump = AUTO, .resistance = AUTO}),
    Term("term 2", Duration{8, 29, 26}, Actions{.pump = OFF, .resistance = OFF}),
};

Term V_40_TERMS[] = {
    Term("term 1", Duration{11, 35, 0}, Actions{.pump = AUTO, .resistance = AUTO}),
    Term("term 2", Duration{0, 30, 0}, Actions{.pump = ON, .resistance = ON}),
    Term("term 3", Duration{0, 45, 0}, Actions{.pump = OFF, .resistance = OFF}),
};

// Term TEST_TERMS[] = {
//     Term("term 1", Duration{0, 0, 10}, Actions{.pump = ON, .resistance = ON}),
//     Term("term 2", Duration{0, 0, 10}, Actions{.pump = ON, .resistance = ON}),
//     Term("term 3", Duration{0, 0, 10}, Actions{.pump = OFF, .resistance = OFF}),
// };

// We define the cycles
Cycle CYCLES[] = {
    // Cycle("TEST Cycle", 3, TEST_TERMS),
    Cycle("IDLING Cycle", 3, IDLING_TERMS, true), // We set the start condition to true
    Cycle("DRAINING Cycle", 2, IDLING_TERMS),
    Cycle("V_40 Cycle", 3, V_40_TERMS),
};

// We define the manager
Manager CycleManager(3, CYCLES);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
}

void loop()
{
  CycleManager.run();
}