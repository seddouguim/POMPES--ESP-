#ifndef SRC_CONFIG
#define SRC_CONFIG

#include <Arduino.h>

// DEBUG
#define DEBUG true
#define DEBUG_RATIO 360

// NETWORK
#define WIFI_SSID "UPC2703909"
#define WIFI_PASSWORD "nxkVF7ksvt8j"

// ESP PINS
#define BUZZER_PIN 13
#define RESISTANCE_PIN 5
#define PUMP_PIN 4

// THERMOCOUPLE PINS
#define CLK 14
#define CS 0
#define MISO 12 // DO

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

#define Screen Serial1

// MISCELLANEOUS
#define MESSAGE_NOT_DISPLAYED 0
#define MESSAGE_DISPLAYED 1

#define CYCLE_BEEP_AMOUNT 3
#define TERM_BEEP_AMOUNT 1

#endif /* SRC_CONFIG */
