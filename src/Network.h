#ifndef SRC_NETWORK
#define SRC_NETWORK

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "State.h"
#include "secrets.h"

#define cuid "claay207y00083b6qint7umkc"

class Network
{
public:
    Network();
    void loop(State *state);

private:
    bool initialized;
    String WIFI_ssid;
    String WIFI_password;

    // Reference to State
    State *state;

    void init();

    void connect_wifi();
    void connect_mqtt();

    // Network Related
    WiFiClientSecure wifi_client;
    PubSubClient mqtt_client;

    BearSSL::X509List cert;
    BearSSL::X509List client_crt;
    BearSSL::PrivateKey key;

    // MQTT Related
    String THING_NAME;
    String MQTT_HOST;
    String AWS_IOT_PUBLISH_TOPIC;
    String AWS_IOT_SUBSCRIBE_TOPIC;
    void publish_message();
    unsigned long last_publish_time;

    // TIME RELATED
    time_t now;
    time_t nowish;
    int TIME_ZONE;
    void get_time(void);
};

#endif /* SRC_NETWORK */
