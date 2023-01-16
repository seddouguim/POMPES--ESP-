#ifndef SRC_CONFIG
#define SRC_CONFIG

#include <Arduino.h>

// ESP PINS
#define BUZZER_PIN 13
#define RESISTANCE_PIN 5
#define PUMP_PIN 4

// THERMOCOUPLE PINS
#define CLK 14
#define CS 15
#define MISO 12

// TEMPERATURE VARIABLES
extern float MIN_TEMPERATURE;
extern float MAX_TEMPERATURE;
extern float PUMP_ON;
extern float PUMP_OFF;

// SCREEN VARIABLES
#define GREEN 960
#define RED 47169
#define BLUE 343

#define RESISTANCE_OFF_PIC 81
#define PUMP_OFF_PIC 80

#endif /* SRC_CONFIG */
