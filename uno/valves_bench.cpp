/*
Program for the UNO.

1) Deals with receiving the draining signal from the NANO
2) Activates/deactivates valves based on specific timings.

*/

#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define relayV3L 2
#define relayV4L 4
#define relayV6L 7
#define relayV10L 8

RTC_DS3231 rtc;
SoftwareSerial mySerial(10, 11); // RX, TX
LiquidCrystal_I2C lcd(0x27, 20, 4);

char currentTime[12];
char currentDate[12];
unsigned long currentTimestamp = 0ul;
unsigned long lastTimeDisplay = 0ul;

unsigned long lastValveStartTime = 0ul;

bool receivedActivationSignalFromNano = false;
bool receivedLastValveSignalFromNano = false;

struct Timing
{
    char startTime[12];
    char endTime[12];
    int relayPin;
} Timing;

/*------------------------------DRAINING SCHEDULE------------------------------*/

// struct Timing schedule[] = {
//     {"07:00:00","07:00:34",relayV3L},
//     {"07:05:00","07:05:17",relayV6L},
//     {"07:30:00","07:30:34",relayV3L},
//     {"07:45:00","07:45:34",relayV3L},
//     {"08:05:00","08:11:30",relayV10L},
//     {"08:25:00","08:25:38",relayV3L},
//     {"08:30:00","08:30:38",relayV3L},
//     {"08:45:00","08:45:36",relayV3L},
//     {"09:00:00","09:00:34",relayV3L},
//     {"09:30:00","09:30:34",relayV3L},
//     {"10:30:00","10:30:34",relayV3L},
//     {"11:30:00","11:30:38",relayV3L},
//     {"11:45:00","12:45:38",relayV3L},
//     {"12:45:00","11:46:35",relayV3L},
//     {"14:30:00","14:30:36",relayV3L},
//     {"15:30:00","15:30:38",relayV3L},
//     {"16:30:00","16:30:38",relayV3L},
//     {"18:00:00","18:00:40",relayV3L},
//     {"18:15:00","18:15:36",relayV3L},
//     {"18:30:00","18:30:34",relayV3L},
//     {"19:00:00","19:00:36",relayV3L},
//     {"20:30:00","20:33:02",relayV4L},
//     {"21:00:00","21:06:20",relayV10L},
//     {"21:30:00","21:30:34",relayV3L},
// };

// TEST SCHEDULE
struct Timing schedule[] = {
    {"07:00:00", "07:00:10", relayV3L},
    {"07:00:15", "07:00:20", relayV6L},
    {"07:00:25", "07:00:30", relayV3L},
    {"07:00:35", "07:00:40", relayV3L},
    {"07:00:45", "07:00:50", relayV10L},
    {"07:01:00", "07:01:10", relayV3L},
    {"07:01:15", "07:01:20", relayV3L},
    {"07:01:25", "07:01:30", relayV3L},
    {"07:01:35", "07:01:40", relayV3L},
    {"07:01:45", "07:01:50", relayV3L},
    {"07:02:00", "07:02:10", relayV3L},
    {"07:02:15", "07:02:20", relayV3L},
    {"07:02:25", "07:02:30", relayV3L},
    {"07:02:35", "07:02:40", relayV3L},
    {"07:02:45", "07:02:50", relayV3L},
    {"07:03:00", "07:03:10", relayV3L},
    {"07:03:15", "07:03:20", relayV3L},
    {"07:03:25", "07:03:30", relayV3L},
    {"07:03:35", "07:03:40", relayV3L},
    {"07:03:45", "07:03:50", relayV3L},
    {"07:04:00", "07:04:10", relayV3L},
    {"07:04:15", "07:04:20", relayV4L},
    {"07:04:25", "07:04:30", relayV10L},
    {"07:04:35", "07:04:40", relayV3L},
};

void setup()
{
    Serial.begin(9600);
    mySerial.begin(9600);

    pinMode(relayV3L, OUTPUT);
    pinMode(relayV4L, OUTPUT);
    pinMode(relayV6L, OUTPUT);
    pinMode(relayV10L, OUTPUT);

    if (!rtc.begin())
        Serial.println("Couldn't find RTC");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    lcd.init();      // Initialise l'afficheur LCD
    lcd.backlight(); // Active le rétro-éclairage
    lcd.clear();
}

void loop()
{
    DateTime now = rtc.now();
    currentTimestamp = millis();

    sprintf(currentTime, "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    sprintf(currentDate, "%02u/%02u/%02u", now.day(), now.month(), now.year());

    displayTime();
    if (receivedActivationSignalFromNano)
        watchTimings();
    serialListen();
    handleLastValve();
}

void watchTimings()
{
    for (int i = 0; i < 24; i++)
    {
        if (!strcmp(currentTime, schedule[i].startTime))
        {
            if (!digitalRead(schedule[i].relayPin))
            {
                digitalWrite(schedule[i].relayPin, HIGH);
                displayValveMessage(schedule[i].relayPin, 1);
                Serial.println("Valve opened.");
            }
        }
        else if (!strcmp(currentTime, schedule[i].endTime))
        {
            if (digitalRead(schedule[i].relayPin))
            {
                digitalWrite(schedule[i].relayPin, LOW);
                displayValveMessage(schedule[i].relayPin, 0);
                Serial.println("Valve closed.");
            }
        }
    }
}

void listen_for_commands()
{
    static bool rx = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (ScreenRX.available() > 0)
    {
        rc = ScreenRX.read();

        if (rx == true)
        {
            if (rc != endMarker)
            {
                input_buffer[ndx] = rc;
                ndx++;
                if (ndx >= INPUT_BUFFER_SIZE)
                {
                    ndx = INPUT_BUFFER_SIZE - 1;
                }
            }
            else
            {
                input_buffer[ndx] = '\0'; // terminate the string
                rx = false;
                ndx = 0;

                handle_command(String(input_buffer));
            }
        }

        else if (rc == startMarker)
        {
            rx = true;
        }
    }
}

void handle_command(String command)
{

    if (command == "A" && !receivedActivationSignalFromNano)
    {
        receivedActivationSignalFromNano = true;
        rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), 6, 59, 59)); // SET RTC TIME TO 6h59
        Serial.println("Received activation signal from NANO (Time set to 6h59).");
    }

    else if (command == "B" && !receivedLastValveSignalFromNano)
    {
        lastValveStartTime = currentTimestamp;
        receivedLastValveSignalFromNano = true;
        if (!digitalRead(relayV10L))
        {
            digitalWrite(relayV10L, HIGH);
            displayValveMessage(relayV10L, 1);
        }

        Serial.println("Received last valve activation signal from NANO.");
    }
}

void serialListen()
{
    if (mySerial.available() > 0)
    {
        incomingCharFromNano = mySerial.read();

        if (incomingCharFromNano == 'B' && !receivedLastValveSignalFromNano)
        {
        }

        else if (incomingCharFromNano == 'A' && !receivedActivationSignalFromNano)
        {
            receivedActivationSignalFromNano = true;
            rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), 6, 59, 59)); // SET RTC TIME TO 6h59
            Serial.println("Received activation signal from NANO (Time set to 6h59).");
        }
    }
}

void handleLastValve()
{
    if (!receivedLastValveSignalFromNano)
        return;
    if (currentTimestamp - lastValveStartTime > 1800000)
    { // 30 minutes last valve
        if (digitalRead(relayV10L))
        {
            digitalWrite(relayV10L, LOW);
            displayValveMessage(relayV10L, 0);
            Serial.println("Closed last Valve.");
            Serial.println("Programme UNO terminé.");
        }
        receivedActivationSignalFromNano = false;
        receivedLastValveSignalFromNano = false;
    }
}

void displayValveMessage(int pinNumber, int state)
{
    lcd.setCursor(0, 2);
    lcd.print("                   ");
    lcd.setCursor(0, 2);

    switch (pinNumber)
    {
    case relayV3L:
        lcd.print("3L/min ");
        break;
    case relayV4L:
        lcd.print("4L/min ");
        break;
    case relayV6L:
        lcd.print("6L/min ");
        break;
    case relayV10L:
        lcd.print("10L/min ");
        break;
    default:
        break;
    }
    lcd.print(state ? "OPENED at:" : "CLOSED at:");
    lcd.setCursor(0, 3);
    lcd.print("           ");
    lcd.print(currentTime);
}

void displayTime()
{
    if (currentTimestamp - lastTimeDisplay < 1000)
        return;
    lastTimeDisplay = currentTimestamp;
    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    lcd.print(currentDate);
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(currentTime);
}
