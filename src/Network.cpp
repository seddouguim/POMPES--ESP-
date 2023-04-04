#include "Network.h"
#include "Display.h"

Network::Network()
    : initialized(false), state(nullptr), wifi_client(), mqtt_client(wifi_client),
      cert(AWS_CERT_CA), client_crt(AWS_CERT_CRT), key(AWS_CERT_PRIVATE)
{
    WIFI_ssid = "";
    WIFI_password = "";

    connection_attempts = 0;

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

    get_wifi_credentials();
    initialized = true;
}

void Network::get_wifi_credentials()
{
    // Get wifi credentials from file (if they exist)
    File file = LittleFS.open("/wifi/config.txt", "r");
    if (file)
    {
        // file.println() => \r\n
        WIFI_ssid = file.readStringUntil('\n');
        WIFI_password = file.readStringUntil('\n');

        // Remove trailing \r
        WIFI_ssid.trim();
        WIFI_password.trim();

        file.close();
    }
}

void Network::save_wifi_credentials(String ssid, String password)
{
    // Save wifi credentials to file
    File file = LittleFS.open("/wifi/config.txt", "w");
    if (file)
    {
        // save in ssid:password format
        file.println(ssid);
        file.println(password);

        file.close();
    }

    // Update wifi credentials
    WIFI_ssid = ssid;
    WIFI_password = password;
    connection_attempts = 0;
}

void Network::clear_wifi_credentials()
{
    WIFI_ssid = "";
    WIFI_password = "";
    connection_attempts = 0;
}

void Network::connect_wifi()
{
    // Check if we're connected to WiFi already
    // Also check if we've tried to connect more than once
    if (WiFi.status() == WL_CONNECTED || connection_attempts >= 1)
        return;

    // Update WiFi icon
    Display::update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    connection_attempts++;
    unsigned long start_time = millis();

    // If we're not connected, we try to connect
    // nxkVF7ksvt8j
    // UPC2703909
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->WIFI_ssid, this->WIFI_password);

    // Update display
    // @color: ORANGE
    Display::update_display("main.state.txt=\"Connecting to WiFi...\"");
    Display::update_display("main.state.pco=" + String(ORANGE));

    // Wait for connection or timeout
    while (WiFi.status() != WL_CONNECTED && millis() - start_time < CONNECTION_TIMEOUT)
        delay(500);

    // If we're still not connected, we return
    if (WiFi.status() != WL_CONNECTED)
        return;

    Display::update_display("main.wifi.pic=" + String(WIFI_CONNECTED_PIC));
    connection_attempts = 0;
}

void Network::connect_mqtt()
{
    // Check if we're connected to MQTT already
    if (mqtt_client.connected())
        return;

    // Update display
    // @color: ORANGE
    Display::update_display("main.state.txt=\"Connecting to AWS...\"");
    Display::update_display("main.state.pco=" + String(ORANGE));

    unsigned long start_time = millis();

    // Wait for connection or timeout
    while (!mqtt_client.connect(THING_NAME.c_str()) && millis() - start_time < CONNECTION_TIMEOUT)
        delay(1000);

    // AWS connection timeout
    if (!mqtt_client.connected())
        return;

    // Subscribe to a topic
    // mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC.c_str());
}

void Network::publish_messages()
{
    connect_mqtt();

    publish_live_data_message();
    publish_database_message();
}

void Network::publish_live_data_message()
{
    // Check if time to publish
    if (millis() - last_live_publish_time < MQTT_PUBLISH_INTERVAL)
        return;

    // Publish an MQTT message to the device's update shadow topic
    // This will update the device's shadow document with the current state
    mqtt_client.publish(SHADOW_UPDATE_TOPIC.c_str(), state->get_shadow_update_document());

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

    last_database_publish_time = millis();
}

void Network::get_time(void)
{
    if (now != 0)
        return;

    configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);

    while (now < nowish)
    {
        delay(500);
        now = time(nullptr);
    }

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
}

void Network::loop(State *state)
{
    // Check if state is nullptr
    // If it is, we save a reference to the state
    if (this->state == nullptr)
        this->state = state;

    init();

    // Check if we have wifi credentials
    // If we don't, we return
    if (WIFI_ssid == "")
        return;

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
