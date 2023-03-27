#include "Network.h"

Network::Network()
    : initialized(false), state(nullptr), server(), wifi_client(), mqtt_client(wifi_client),
      cert(AWS_CERT_CA), client_crt(AWS_CERT_CRT), key(AWS_CERT_PRIVATE)
{
    WIFI_ssid = DEV ? WIFI_SSID_DEV : WIFI_SSID;
    WIFI_password = DEV ? WIFI_PASSWORD_DEV : WIFI_PASSWORD;

    THING_NAME = "ESP8266";
    MQTT_HOST = "a2bc1rtj36q5u9-ats.iot.us-east-1.amazonaws.com";

    SHADOW_UPDATE_TOPIC = "$aws/things/" + THING_NAME + "/shadow/update";

    now = 0;
    nowish = 1510592825;
    TIME_ZONE = -5;

    last_live_publish_time = 0ul;
    last_database_publish_time = 0ul;
}

void Network::init()
{
    if (initialized)
        return;

    wifi_client.setTrustAnchors(&cert);
    wifi_client.setClientRSACert(&client_crt, &key);

    mqtt_client.setServer(MQTT_HOST.c_str(), 8883);

    AWS_IOT_PUBLISH_TOPIC = "dt/sensors/" + String(state->CUID);
    AWS_IOT_SUBSCRIBE_TOPIC = "dt/sensors/" + String(state->CUID) + "/sub";

    // Get wifi credentials from file (if they exist)

    // File file = LittleFS.open("/wifi/config.txt", "r");
    // if (file)
    // {
    //     Serial.println("Reading wifi credentials from file.");
    //     WIFI_ssid = file.readStringUntil('\n');
    //     WIFI_password = file.readStringUntil('\n');
    //     file.close();
    // }

    initialized = true;
}

void Network::get_time(void)
{
    if (now != 0)
        return;

    // SERIAL_DEBUG &&
    Serial.print("Setting time using SNTP.");
    configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);
    while (now < nowish)
    {
        delay(500);
        // SERIAL_DEBUG &&
        Serial.print(".");
        now = time(nullptr);
    }

    // SERIAL_DEBUG &&
    Serial.print("done!");
    Serial.println();

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
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

    SERIAL_DEBUG &&
        Serial.print("Attempting to connect to WiFi (SSID: " + WIFI_ssid + ")");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        SERIAL_DEBUG &&
            Serial.print(".");
    }

    SERIAL_DEBUG &&
        Serial.println("Connected to WiFi!");
}

void Network::connect_mqtt()
{
    // Check if we're connected to MQTT
    // If we are, we return
    if (mqtt_client.connected())
        return;

    SERIAL_DEBUG &&
        Serial.println("Attempting to connect to AWS IOT.");

    while (!mqtt_client.connect(THING_NAME.c_str()))
    {
        Serial.print(".");
        delay(1000);
    }

    if (!mqtt_client.connected())
    {
        SERIAL_DEBUG &&
            Serial.println("AWS IoT Timeout!");
        return;
    }
    // Subscribe to a topic
    mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC.c_str());

    SERIAL_DEBUG &&
        Serial.println("AWS IoT Connected!");

    SERIAL_DEBUG &&
        Serial.println();
}

void Network::publish_live_data_message()
{
    // Check if time to publish
    if (millis() - last_live_publish_time < MQTT_PUBLISH_INTERVAL)
        return;

    // Publish an MQTT message to the device's update shadow topic
    // This will update the device's shadow document with the current state

    mqtt_client.publish(SHADOW_UPDATE_TOPIC.c_str(), state->get_shadow_update_document());

    SERIAL_DEBUG &&
        Serial.println("Published to: " + SHADOW_UPDATE_TOPIC);

    last_live_publish_time = millis();
}

void Network::publish_database_message()
{
    // Check if time to publish
    if (millis() - last_database_publish_time < MQTT_DATABASE_PUBLISH_INTERVAL && last_database_publish_time != 0ul)
        return;

    // Publish an MQTT message for database
    String topic = "database/" + AWS_IOT_PUBLISH_TOPIC;
    mqtt_client.publish(topic.c_str(), state->get_state_json());

    SERIAL_DEBUG &&
        Serial.println("Data saved to database.");

    SERIAL_DEBUG &&
        Serial.println("Published to: " + topic);

    last_database_publish_time = millis();
}

void Network::publish_messages()
{
    connect_mqtt();

    publish_live_data_message();
    publish_database_message();
}

void Network::loop(State *state)
{
    // Check if state is nullptr
    // If it is, we save a reference to the state
    if (this->state == nullptr)
        this->state = state;

    init();

    // Check if we have wifi credentials
    // If we don't, we start the internal server and return
    if (WIFI_ssid == "" || WIFI_password == "")
    {
        server.start();
        return;
    }

    else if (server.is_running())
    {
        // If the internal server is running, we check if we have wifi credentials
        if (server.get_wifi_credentials_saved())
        {
            SERIAL_DEBUG &&
                Serial.println("Wifi credentials saved.");

            WIFI_ssid = server.get_WIFI_ssid();
            WIFI_password = server.get_WIFI_password();

            // we stop the internal server
            server.end();
        }

        // If we still don't have wifi credentials, we return.
        // Meaning, we exit the whole loop and don't connect to the wifi yet.
        return;
    }

    connect_wifi();

    // Check if we're connected to WiFi
    // If we're not, we return
    if (WiFi.status() != WL_CONNECTED)
        return;

    get_time();
    now = time(nullptr);

    publish_messages();

    mqtt_client.loop();
}
