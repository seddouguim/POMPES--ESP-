#include "State.h"

State::State()
    : Thermocouple(MAX6675(MAX_CLK, MAX_CS, MAX_MISO)),
      Thermocouple_max31855(Adafruit_MAX31855(MAX_CLK, MAX_CS, MAX_MISO))
{
    init();
}

void State::init()
{
    if (!DEV)
        Thermocouple_max31855.begin();

    current_temperature = 0;
    previous_temperature = -1;

    resistance_state = false;
    previous_resistance_state = false;

    pump_state = false;
    previous_pump_state = false;

    pump_start_timer = 0;
    resistance_start_timer = 0;

    daily_pump_kwh = 0;
    daily_resistance_kwh = 0;
    monthly_pump_kwh = 0;
    monthly_resistance_kwh = 0;

    pump_kwh = 0;
    resistance_kwh = 0;

    current_cycle = "";
    current_term = "";

    last_update = 0;

    pinMode(RESISTANCE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
}

float State::get_calibrated_temperature()
{

    // MAX31855 thermocouple voltage reading in mV
    float thermocoupleVoltage = (Thermocouple_max31855.readCelsius() - Thermocouple_max31855.readInternal()) * 0.041276;

    // MAX31855 cold junction voltage reading in mV
    float coldJunctionTemperature = Thermocouple_max31855.readInternal();
    float coldJunctionVoltage = -0.176004136860E-01 +
                                0.389212049750E-01 * coldJunctionTemperature +
                                0.185587700320E-04 * pow(coldJunctionTemperature, 2.0) +
                                -0.994575928740E-07 * pow(coldJunctionTemperature, 3.0) +
                                0.318409457190E-09 * pow(coldJunctionTemperature, 4.0) +
                                -0.560728448890E-12 * pow(coldJunctionTemperature, 5.0) +
                                0.560750590590E-15 * pow(coldJunctionTemperature, 6.0) +
                                -0.320207200030E-18 * pow(coldJunctionTemperature, 7.0) +
                                0.971511471520E-22 * pow(coldJunctionTemperature, 8.0) +
                                -0.121047212750E-25 * pow(coldJunctionTemperature, 9.0) +
                                0.118597600000E+00 * exp(-0.118343200000E-03 *
                                                         pow((coldJunctionTemperature - 0.126968600000E+03), 2.0));

    // cold junction voltage + thermocouple voltage
    float voltageSum = thermocoupleVoltage + coldJunctionVoltage;

    // calculate corrected temperature reading based on coefficients for 3 different ranges
    float b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10;
    if (thermocoupleVoltage < 0)
    {
        b0 = 0.0000000E+00;
        b1 = 2.5173462E+01;
        b2 = -1.1662878E+00;
        b3 = -1.0833638E+00;
        b4 = -8.9773540E-01;
        b5 = -3.7342377E-01;
        b6 = -8.6632643E-02;
        b7 = -1.0450598E-02;
        b8 = -5.1920577E-04;
        b9 = 0.0000000E+00;
    }

    else if (thermocoupleVoltage < 20.644)
    {
        b0 = 0.000000E+00;
        b1 = 2.508355E+01;
        b2 = 7.860106E-02;
        b3 = -2.503131E-01;
        b4 = 8.315270E-02;
        b5 = -1.228034E-02;
        b6 = 9.804036E-04;
        b7 = -4.413030E-05;
        b8 = 1.057734E-06;
        b9 = -1.052755E-08;
    }

    else if (thermocoupleVoltage < 54.886)
    {
        b0 = -1.318058E+02;
        b1 = 4.830222E+01;
        b2 = -1.646031E+00;
        b3 = 5.464731E-02;
        b4 = -9.650715E-04;
        b5 = 8.802193E-06;
        b6 = -3.110810E-08;
        b7 = 0.000000E+00;
        b8 = 0.000000E+00;
        b9 = 0.000000E+00;
    }

    else
    {
        // TODO: handle error - out of range
        return 0;
    }

    return b0 +
           b1 * voltageSum +
           b2 * pow(voltageSum, 2.0) +
           b3 * pow(voltageSum, 3.0) +
           b4 * pow(voltageSum, 4.0) +
           b5 * pow(voltageSum, 5.0) +
           b6 * pow(voltageSum, 6.0) +
           b7 * pow(voltageSum, 7.0) +
           b8 * pow(voltageSum, 8.0) +
           b9 * pow(voltageSum, 9.0);
}

void State::update()
{
    if (millis() - last_update < 1000)
        return;

    last_update = millis();

    previous_temperature = current_temperature;
    current_temperature = get_calibrated_temperature();
    // current_temperature = Thermocouple_max31855.readCelsius();
    // current_temperature = random(10, 100);

    previous_resistance_state = resistance_state;
    resistance_state = digitalRead(RESISTANCE_PIN);

    previous_pump_state = pump_state;
    pump_state = digitalRead(PUMP_PIN);

    calculate_kws();
}

char *State::get_state_json()
{
    StaticJsonDocument<300> doc;

    doc["device_id"] = CUID;
    doc["current_temperature"] = isnan(current_temperature) ? -1 : current_temperature;
    doc["resistance_state"] = resistance_state;
    doc["pump_state"] = pump_state;
    doc["current_cycle"] = current_cycle;
    doc["current_term"] = current_term;
    doc["pump_power"] = pump_state ? PUMP_CONSUMPTION : 0;
    doc["resistance_power"] = resistance_state ? RESISTANCE_CONSUMPTION : 0;
    doc["pump_on_time_total"] = -1;
    doc["resistance_on_time_total"] = -1;
    doc["resistance_kwh"] = resistance_kwh;
    doc["pump_kwh"] = pump_kwh;

    static char state_json[JSON_BUFFER_SIZE];
    serializeJson(doc, state_json);

    return state_json;
}

char *State::get_shadow_update_document()
{
    StaticJsonDocument<200> doc;

    doc["state"]["reported"]["current_temperature"] = current_temperature;
    doc["state"]["reported"]["resistance_state"] = resistance_state;
    doc["state"]["reported"]["pump_state"] = pump_state;
    doc["state"]["reported"]["current_cycle"] = current_cycle;
    doc["state"]["reported"]["current_term"] = current_term;
    doc["state"]["reported"]["current_term_duration"] = current_term_duration;
    doc["state"]["reported"]["current_cycle_duration"] = current_cycle_duration;
    doc["state"]["reported"]["pump_power"] = pump_state ? PUMP_CONSUMPTION : 0;
    doc["state"]["reported"]["resistance_power"] = resistance_state ? RESISTANCE_CONSUMPTION : 0;

    static char shadow_update_document[JSON_BUFFER_SIZE];
    serializeJson(doc, shadow_update_document);

    return shadow_update_document;
}

void State::calculate_kws()
{
    pump_kw();
    resistance_kw();
}

void State::pump_kw()
{
    if (pump_state == previous_pump_state)
        return;

    // Pump has just turned ON
    // We start the pump timer
    if (pump_state)
        pump_start_timer = millis();

    // Pump has just turned OFF
    // We calculate the time the PUMP has been on
    else
    {
        unsigned long pump_on_time_interval = millis() - pump_start_timer;

        // We calculate the number of kilowatt-hours
        pump_kwh = (pump_on_time_interval * PUMP_CONSUMPTION) / (1000UL * 3600UL * 1000UL);
    }
}

void State::resistance_kw()
{
    if (resistance_state == previous_resistance_state)
        return;

    // Resistance has just turned ON
    // We start the resistance timer
    if (resistance_state)
        resistance_start_timer = millis();

    // Resistance has just turned OFF
    // We calculate the time the resistance has been on
    else
    {
        unsigned long resistance_on_time_interval = millis() - resistance_start_timer;

        // We calculate the number of kilowatt-hours
        resistance_kwh = (resistance_on_time_interval * RESISTANCE_CONSUMPTION) / (1000UL * 3600UL * 1000UL);
    }
}

// GET_UPATE FUNCTIONS
// RETURN TRUE IF EACH PREVIOUS STATE IS DIFFERENT FROM CURRENT STATE
bool State::get_temperature_update()
{
    return current_temperature != previous_temperature;
}

bool State::get_resistance_update()
{
    return resistance_state != previous_resistance_state;
}

bool State::get_pump_update()
{
    return pump_state != previous_pump_state;
}
