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
    const int numReadings = 10; // Number of readings to average
    float sum = 0.0;

    for (int i = 0; i < numReadings; i++)
    {
        // MAX31855 thermocouple voltage reading in mV
        float thermocoupleVoltage = (Thermocouple_max31855.readCelsius() - Thermocouple_max31855.readInternal()) * 0.041276;
        sum += thermocoupleVoltage;
        delay(10); // Add a small delay between readings to stabilize the sensor
    }

    float averageVoltage = sum / numReadings;

    // Calculate the Exponential Moving Average (EMA)
    const float smoothingFactor = 0.2;         // Smoothing factor (0 < smoothingFactor < 1)
    static float previousEMA = averageVoltage; // Initial EMA value

    float currentEMA = smoothingFactor * averageVoltage + (1 - smoothingFactor) * previousEMA;
    previousEMA = currentEMA; // Update the previous EMA value

    // Rest of the calibration code using the EMA value
    float calibratedTemperature = 0.0;

    // Use the EMA value to perform the temperature calibration
    if (currentEMA < 0)
    {
        // Calibration coefficients for negative thermocouple voltage
        float b0 = 0.0000000E+00;
        float b1 = 2.5173462E+01;
        float b2 = -1.1662878E+00;
        float b3 = -1.0833638E+00;
        float b4 = -8.9773540E-01;
        float b5 = -3.7342377E-01;
        float b6 = -8.6632643E-02;
        float b7 = -1.0450598E-02;
        float b8 = -5.1920577E-04;
        float b9 = 0.0000000E+00;

        calibratedTemperature = b0 +
                                b1 * currentEMA +
                                b2 * pow(currentEMA, 2.0) +
                                b3 * pow(currentEMA, 3.0) +
                                b4 * pow(currentEMA, 4.0) +
                                b5 * pow(currentEMA, 5.0) +
                                b6 * pow(currentEMA, 6.0) +
                                b7 * pow(currentEMA, 7.0) +
                                b8 * pow(currentEMA, 8.0) +
                                b9 * pow(currentEMA, 9.0);
    }
    else if (currentEMA < 20.644)
    {
        // Calibration coefficients for thermocouple voltage range 0 to 20.644 mV
        float b0 = 0.000000E+00;
        float b1 = 2.508355E+01;
        float b2 = 7.860106E-02;
        float b3 = -2.503131E-01;
        float b4 = 8.315270E-02;
        float b5 = -1.228034E-02;
        float b6 = 9.804036E-04;
        float b7 = -4.413030E-05;
        float b8 = 1.057734E-06;
        float b9 = -1.052755E-08;

        calibratedTemperature = b0 +
                                b1 * currentEMA +
                                b2 * pow(currentEMA, 2.0) +
                                b3 * pow(currentEMA, 3.0) +
                                b4 * pow(currentEMA, 4.0) +
                                b5 * pow(currentEMA, 5.0) +
                                b6 * pow(currentEMA, 6.0) +
                                b7 * pow(currentEMA, 7.0) +
                                b8 * pow(currentEMA, 8.0) +
                                b9 * pow(currentEMA, 9.0);
    }
    else if (currentEMA < 54.886)
    {
        // Calibration coefficients for thermocouple voltage range 20.644 to 54.886 mV
        float b0 = -1.318058E+02;
        float b1 = 4.830222E+01;
        float b2 = -1.646031E+00;
        float b3 = 5.464731E-02;
        float b4 = -9.650715E-04;
        float b5 = 8.802193E-06;
        float b6 = -3.110810E-08;
        float b7 = 0.000000E+00;
        float b8 = 0.000000E+00;
        float b9 = 0.000000E+00;

        calibratedTemperature = b0 +
                                b1 * currentEMA +
                                b2 * pow(currentEMA, 2.0) +
                                b3 * pow(currentEMA, 3.0) +
                                b4 * pow(currentEMA, 4.0) +
                                b5 * pow(currentEMA, 5.0) +
                                b6 * pow(currentEMA, 6.0) +
                                b7 * pow(currentEMA, 7.0) +
                                b8 * pow(currentEMA, 8.0) +
                                b9 * pow(currentEMA, 9.0);
    }
    else
    {
        // Handle error - out of range
        calibratedTemperature = previous_temperature;
    }

    return calibratedTemperature; // Return the calibrated temperature
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
