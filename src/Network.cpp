#include "Network.h"

Network::Network()
    : initialized(false), state(nullptr), wifi_client(), mqtt_client(wifi_client),
      cert(AWS_CERT_CA), client_crt(AWS_CERT_CRT), key(AWS_CERT_PRIVATE)
{
    WIFI_ssid = WIFI_SSID;
    WIFI_password = WIFI_PASSWORD;

    THING_NAME = "ESP8266";
    MQTT_HOST = "a2bc1rtj36q5u9-ats.iot.us-east-1.amazonaws.com";

    now = 0;
    nowish = 1510592825;
    TIME_ZONE = -5;

    last_publish_time = 0ul;
}

void Network::init()
{
    if (initialized)
        return;

    get_time();
    wifi_client.setTrustAnchors(&cert);
    wifi_client.setClientRSACert(&client_crt, &key);

    mqtt_client.setServer(MQTT_HOST.c_str(), 8883);

    AWS_IOT_PUBLISH_TOPIC = "dt/sensors/" + String(state->CUID);
    AWS_IOT_SUBSCRIBE_TOPIC = "dt/sensors/" + String(state->CUID) + "/sub";

    initialized = true;
}

void Network::get_time(void)
{
    Serial.print("Setting time using SNTP.");
    configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);
    while (now < nowish)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.print("done!");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);

    Serial.println();
    Serial.print("Current time: " + String(asctime(&timeinfo)));
}

void Network::connect_wifi()
{
    // Check if we're connected to WiFi
    // If we are, we return
    if (WiFi.status() == WL_CONNECTED)
        return;

    // If we're not connected, we try to connect
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_ssid, WIFI_password);

    Serial.print("Attempting to connect to WiFi (SSID: " + WIFI_ssid + ")");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to the WiFi network");
}

void Network::connect_mqtt()
{
    // Check if we're connected to MQTT
    // If we are, we return
    if (mqtt_client.connected())
        return;

    Serial.println("Connecting to AWS IOT.");

    while (!mqtt_client.connect(THING_NAME.c_str()))
    {
        Serial.print(".");
        delay(1000);
    }

    if (!mqtt_client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }
    // Subscribe to a topic
    mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC.c_str());

    Serial.println("AWS IoT Connected!");
    Serial.println();
}

void Network::publish_message()
{
    // Check if time to publish
    if (millis() - last_publish_time < MQTT_PUBLISH_INTERVAL)
        return;

    // Publish an MQTT message
    mqtt_client.publish(AWS_IOT_PUBLISH_TOPIC.c_str(), state->get_state_json());

    Serial.println("Published to: " + AWS_IOT_PUBLISH_TOPIC);
    last_publish_time = millis();
}

void Network::loop(State *state)
{
    // Check if state is nullptr
    // If it is, we save a reference to the state
    if (this->state == nullptr)
        this->state = state;

    connect_wifi();

    // Check if we're connected to WiFi
    // If we're not, we return
    if (WiFi.status() != WL_CONNECTED)
        return;

    init();

    now = time(nullptr);

    connect_mqtt();

    publish_message();

    mqtt_client.loop();
}
