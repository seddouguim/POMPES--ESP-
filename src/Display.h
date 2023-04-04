#ifndef SRC_DISPLAY
#define SRC_DISPLAY

#include <Arduino.h>
#include "config.h"
#include "utils.h"

class Manager;

#define INPUT_BUFFER_SIZE 50

class Display
{
public:
    Display();
    void loop();
    static void update_display(String command);
    static void update_state();
    static Manager *manager;

private:
    bool initialized;
    void init();

    Page current_page;

    // TIMERS
    unsigned long last_update;
    unsigned long last_wifi_scan;

    // WIFI
    int ssids_count;
    int current_ssids_page;

    // COMMANDS
    char input_buffer[INPUT_BUFFER_SIZE];
    void listen_for_commands();
    void handle_command(Command command, String value);

    // COMMAND HANDLERS
    void get_wifi_list(String value);
    void display_ssid_list(String page);
    void connect_to_wifi(String credentials);
    void disconnect_from_wifi();
    void set_page(String page);

    // UPDATE FUNCTIONS
    void update_resistance();
    void update_pump();
    void update_temperature();
};

#endif /* SRC_DISPLAY */
