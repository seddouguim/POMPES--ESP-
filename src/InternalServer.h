#ifndef SRC_INTERNAL_SERVER
#define SRC_INTERNAL_SERVER

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"

class InternalServer
{
public:
    InternalServer();
    void start();
    void end();
    bool is_running();

    bool get_wifi_credentials_saved();
    String get_WIFI_ssid();
    String get_WIFI_password();

private:
    String ssid;
    String password;

    String WIFI_ssid;
    String WIFI_password;

    bool initialized;
    bool wifi_credentials_saved;

    AsyncWebServer *server;

    void handle_post(AsyncWebServerRequest *request);
};

#endif /* SRC_INTERNAL_SERVER */
