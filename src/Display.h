#ifndef SRC_DISPLAY
#define SRC_DISPLAY

#include <Arduino.h>
#include "config.h"
#include "utils.h"

class Manager;

#define INPUT_BUFFER_SIZE 50
#define MESSAGE_QUEUE_SIZE 15

class Display
{
public:
    Display();
    void loop();

    static Manager *manager;

    static void update_display(String command);
    static void update_state();
    static void add_message(Message message);

private:
    bool initialized;
    void init();

    Page current_page;
    static Message message_queue[MESSAGE_QUEUE_SIZE];
    static int message_queue_length;

    // TIMERS
    unsigned long last_update;
    unsigned long last_wifi_scan;
    static long message_display_start;

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
    static void display_messages();
    void update_cycle_page();
};

#endif /* SRC_DISPLAY */
