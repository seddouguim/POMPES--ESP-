#ifndef SRC_CONFIG
#define SRC_CONFIG

#include <Arduino.h>

//? ----------------------- PROGRAM CONFIG ----------------------------
// Update PROGRAM_MODE depending on which version of the program you want to run:
// - "BENCH" -- communication with the UNO
// - "OFFICIAL" -- no communication with the UNO
// - "USER" -- no communication with the UNO, only two cycles (WARMING UP and DRAINING), with automatic resistance and pump control

#define PROGRAM_MODE BENCH

//? DEBUG
#define DEBUG true
#define DEBUG_RATIO 90
#define DEV false

//? NETWORK
#define MQTT_PUBLISH_INTERVAL 1000
#define MQTT_DATABASE_PUBLISH_INTERVAL 5000
// #define MQTT_DATABASE_PUBLISH_INTERVAL 60000

#define CONNECTION_TIMEOUT 10000

//? ESP PINS
#define BUZZER_PIN D7
#define RESISTANCE_PIN D1
#define PUMP_PIN D2

//? THERMOCOUPLE PINS
#define MAX_CLK 14
#define MAX_CS 0
#define MAX_MISO 12 // DO

//? TEMPERATURE VARIABLES
extern float MIN_TEMPERATURE;
extern float MAX_TEMPERATURE;
extern float PUMP_ON_TEMPERATURE;
extern float PUMP_OFF_TEMPERATURE;

//? ENERGY CONSUMPTION VARIABLES
extern float RESISTANCE_CONSUMPTION;
extern float PUMP_CONSUMPTION;

//? SCREEN VARIABLES
//* Note: After using the Serial.swap(),
//* (RX: D7, TX: D8)
#define ScreenRX Serial
#define ScreenTX Serial
#define NEXTION_END_STRING "\xff\xff\xff"

//? UNO TX
#define UNO_TX Serial1 // TX: D4, No RX => connected to the UNO RX (10) pin

//* COLORS
#define GREEN 1153
#define RED 43365
#define BLUE 343
#define SKYBLUE 42654
#define SALMON 64073
#define ORANGE 62961
#define LIGHTGREEN 38706
#define YELLOW 65504
#define WHITE 65535
#define BLACK 0
#define DARKGREY 17002

//* PICTURES
#define ARROW_UP_PIC 158
#define ARROW_DOWN_PIC 157

#define WIFI_CONNECTED_PIC 4
#define WIFI_DISCONNECTED_PIC 3

#define WIFI_HIGH_PIC 178
#define WIFI_MED_PIC 180
#define WIFI_LOW_PIC 179

//? MISCELLANEOUS
#define MESSAGE_NOT_DISPLAYED 0
#define MESSAGE_DISPLAYED 1

#define CYCLE_BEEP_AMOUNT 3
#define TERM_BEEP_AMOUNT 1

#define JSON_BUFFER_SIZE 512

#endif /* SRC_CONFIG */
