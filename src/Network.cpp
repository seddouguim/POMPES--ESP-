#include "Network.h"
#include "Display.h"

Network::Network()
    : initialized(false), state(nullptr), wifi_client(), http_client(), mqtt_client(wifi_client),
      cert(AWS_CERT_CA), client_crt(AWS_CERT_CRT), key(AWS_CERT_PRIVATE)
{

    WIFI_ssid = "";
    WIFI_password = "";
    connection_attempts = 0;
    last_mqtt_reconnect_attempt = 0ul;

    // WIFI_ssid = "UPC3404214";
    // WIFI_password = "Y66aedjtudhw";

    // WIFI_ssid = "UPC2703909";
    // WIFI_password = "nxkVF7ksvt8j";

    THING_NAME = "ESP01";
    MQTT_HOST = "a2bc1rtj36q5u9-ats.iot.us-east-1.amazonaws.com";

    SHADOW_UPDATE_TOPIC = "$aws/things/" + THING_NAME + "/shadow/update";

    last_live_publish_time = 0ul;
    last_database_publish_time = 0ul;

    consumption_data = false;
}

void Network::init()
{
    if (initialized)
        return;

    wifi_client.setTrustAnchors(&cert);
    wifi_client.setClientRSACert(&client_crt, &key);

    mqtt_client.setServer(MQTT_HOST.c_str(), 8883);
    mqtt_client.setBufferSize(384);

    AWS_IOT_PUBLISH_TOPIC = "dt/sensors/" + String(state->CUID);
    AWS_IOT_SUBSCRIBE_TOPIC = "dt/sensors/" + String(state->CUID) + "/sub";

    get_wifi_credentials();
    initialized = true;
}

void Network::get_wifi_credentials()
{
    if (WIFI_ssid != "")
        return;

    // Get wifi credentials from config file
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

void Network::check_wifi_connection() {}
void Network::check_mqtt_connection()
{
    if (!mqtt_client.connected())
    {
        long now = millis();
        if (now - last_mqtt_reconnect_attempt > 10000)
        {
            last_mqtt_reconnect_attempt = now;
            // Attempt to reconnect
            if (reconnect_mqtt())
            {
                last_mqtt_reconnect_attempt = 0ul;
            }
        }
    }
    else
    {
        // Client connected
        mqtt_client.loop();

        // Publish messages
        publish_messages();
    }
}

void Network::check_connections()
{
    check_wifi_connection();
    check_mqtt_connection();

    int wifi_status = WiFi.status();
    int mqtt_status = mqtt_client.state();

    if ((wifi_status) != WL_CONNECTED)
    {
        connect_wifi();
        return;
    }

    else if ((wifi_status) != WL_CONNECTED)
    {
    }
}

void Network::connect_wifi()
{
    // Check if we're already connected to WiFi or if we have already attempted connection
    if (WiFi.status() == WL_CONNECTED)
        return;

    // Update WiFi icon
    Display::update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    // Send temporary message to display
    Message connection_message{"Connecting to WiFi...", 1};
    Display::add_message(connection_message);

    // Start WiFi connection in the background
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_ssid.c_str(), WIFI_password.c_str());

    // Set a callback function to handle the WiFi connection status
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info)
                 { handle_wifi_event(event, info); });

    // Reset the connection attempts count
    connection_attempts = 0;
}

void Network::handle_wifi_event(WiFiEvent_t event, WiFiEventInfo_t info)
{
    if (event == SYSTEM_EVENT_STA_CONNECTED)
    {
        // Connected to WiFi
        Display::update_display("main.wifi.pic=" + String(WIFI_CONNECTED_PIC));
        Message connected_message{"Connected to WiFi!", 0};
        Display::add_message(connected_message);
    }
    else if (event == SYSTEM_EVENT_STA_DISCONNECTED)
    {
        // Failed to connect to WiFi
        if (connection_attempts < WIFI_CONNECTION_ATTEMPTS)
        {
            // Retry connection
            connection_attempts++;
            WiFi.reconnect();
        }
        else
        {
            // Max connection attempts reached, show error message
            Message error_message{"Error connecting to WiFi!", 0};
            Display::add_message(error_message);
        }
    }
}

bool Network::reconnect_mqtt()
{
    // Connect to MQTT
    if (mqtt_client.connect(THING_NAME.c_str());)
    {
        // Display success message
        Message connected_message{"Connected to AWS!", 1};
        Display::add_message(connected_message);
    }
    else
    {
        // Display error message
        Message error_message{"Error connecting to AWS!", 1};
        Display::add_message(error_message);
    }

    return client.connected();
}

void Network::publish_messages()
{
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

    // Publish an MQTT message for the database
    String topic = "database/" + AWS_IOT_PUBLISH_TOPIC;
    bool response = mqtt_client.publish(topic.c_str(), state->get_state_json());

    if (!response)
    {
        // Print the error message if publishing fails
        Serial.println("Failed to publish to db, topic: " + topic);
        Serial.println("Error: " + String(mqtt_client.state()));
    }

    else
    {
        // Reset energy consumption counters if they were published with a value > 0
        if (state->pump_kwh > 0 || state->resistance_kwh > 0)
        {

            state->daily_pump_kwh += state->pump_kwh;
            state->monthly_pump_kwh += state->pump_kwh;

            state->daily_resistance_kwh += state->resistance_kwh;
            state->monthly_resistance_kwh += state->resistance_kwh;

            state->pump_kwh = 0;
            state->resistance_kwh = 0;
        }

        last_database_publish_time = millis();
    }
}

void Network::synchronize_time()
{
    configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");

    // Wait for time synchronization or timeout
    unsigned long start_time = millis();
    while (millis() - start_time < TIME_SYNC_TIMEOUT)
    {
        if (time(nullptr) > 0)
            return;
        delay(100);
    }

    // Time synchronization failed
    // Handle the error or retry the synchronization
}

void Network::get_consumption_data()
{
    if (consumption_data)
        return;

    consumption_data = true;

    WiFiClientSecure wifi_client;

    const char *host = "auth.daguok88djgu8.amplifyapp.com";
    const int httpsPort = 443;
    const char *endpoint = "/api/db/power";

    if (!wifi_client.connect(host, httpsPort))
    {
        Serial.println("Failed to connect to API");
        return;
    }

    // Send HTTP GET request
    wifi_client.print(String("GET ") + endpoint + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                      "Connection: close\r\n\r\n");

    // Read and skip response headers
    while (wifi_client.connected())
    {
        String line = wifi_client.readStringUntil('\n');

        if (line == "\r")
            break; // End of HTTP response headers
    }

    // Read the response body
    String payload = wifi_client.readString();

    Serial.println(payload);

    // Disconnect
    wifi_client.stop();

    // Parse JSON response
    DynamicJsonDocument doc(400);
    auto error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print("JSON parsing error: ");
        Serial.println(error.c_str());
        return;
    }

    // Access the JSON data
    double daily_pump_kwh = doc["data"]["daily"]["pump_kwh"].as<double>();
    double daily_resistance_kwh = doc["data"]["daily"]["resistance_kwh"].as<double>();
    double monthly_pump_kwh = doc["data"]["monthly"]["pump_kwh"].as<double>();
    double monthly_resistance_kwh = doc["data"]["monthly"]["resistance_kwh"].as<double>();

    state->daily_pump_kwh = daily_pump_kwh;
    state->daily_resistance_kwh = daily_resistance_kwh;
    state->monthly_pump_kwh = monthly_pump_kwh;
    state->monthly_resistance_kwh = monthly_resistance_kwh;
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

    get_consumption_data();

    publish_messages();

    mqtt_client.loop();
}
