#ifndef SRC_NETWORK
#define SRC_NETWORK

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "State.h"
#include "secrets.h"

class Network
{
public:
    Network();
    void loop(State *state);

    void save_wifi_credentials(String ssid, String password);
    void clear_wifi_credentials();

private:
    bool initialized;

    String WIFI_ssid;
    String WIFI_password;
    int connection_attempts;

    // Reference to State
    State *state;

    void init();

    void get_wifi_credentials();
    void connect_wifi();
    void connect_mqtt();

    void get_consumption_data();
    bool consumption_data;

    // Network Related
    WiFiClientSecure wifi_client;
    HTTPClient http_client;
    PubSubClient mqtt_client;

    BearSSL::X509List cert;
    BearSSL::X509List client_crt;
    BearSSL::PrivateKey key;

    // MQTT Related
    String THING_NAME;
    String MQTT_HOST;
    String AWS_IOT_PUBLISH_TOPIC;
    String AWS_IOT_SUBSCRIBE_TOPIC;

    String SHADOW_UPDATE_TOPIC;

    void publish_messages();

    void publish_live_data_message();
    void publish_database_message();

    unsigned long last_live_publish_time;
    unsigned long last_database_publish_time;

    // TIME RELATED
    time_t now;
    time_t nowish;
    int TIME_ZONE;
    void get_time(void);
};

#endif /* SRC_NETWORK */
