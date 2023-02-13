#include "InternalServer.h"

InternalServer::InternalServer()
{
    ssid = "admin123";
    password = "admin123";

    WIFI_ssid = "";
    WIFI_password = "";

    initialized = false;
}

void InternalServer::start()
{
    // Check if we're already initialized
    // If we are, we return
    if (initialized)
        return;

    Serial.println("No WiFi credentials found. Starting internal server.");

    // Create Wifi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server = new AsyncWebServer(80);

    // Initialize LittleFS
    LittleFS.begin();

    // Server HTML and CSS files from local file system using LittleFS
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "server/index.html", "text/html"); });

    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "server/style.css", "text/css"); });

    // Handle POST request
    server->on("/post", HTTP_POST, [this](AsyncWebServerRequest *request)
               { handle_post(request); });

    // Start server
    server->begin();
    Serial.println("Server started...");

    // Print the IP address
    Serial.println(WiFi.softAPIP());

    initialized = true;
    wifi_credentials_saved = false;
}

void InternalServer::handle_post(AsyncWebServerRequest *request)
{
    // Check if POST request has arguments
    if (request->hasParam("ssid", true) && request->hasParam("password", true))
    {
        // Get arguments
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();

        // Save arguments to file system
        File file = LittleFS.open("/wifi/config.txt", "w");
        file.println(ssid);
        file.println(password);
        file.close();

        // Send response
        request->send(200, "text/plain", "Wifi credentials saved successfully!");

        // print values to serial monitor
        Serial.println(ssid);
        Serial.println(password);

        WIFI_ssid = ssid;
        WIFI_password = password;

        wifi_credentials_saved = true;
    }
    else
    {
        // Send response
        request->send(400, "text/plain", "Bad Request");
    }
}

void InternalServer::end()
{
    server->end();
    initialized = false;
}

bool InternalServer::is_running()
{
    return initialized;
}

bool InternalServer::get_wifi_credentials_saved()
{
    return wifi_credentials_saved;
}

String InternalServer::get_WIFI_ssid()
{
    return WIFI_ssid;
}

String InternalServer::get_WIFI_password()
{
    return WIFI_password;
}
