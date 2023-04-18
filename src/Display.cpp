#include "Display.h"
#include "Manager.h"

// declare the static variables
Manager *Display::manager = nullptr;
Message Display::message_queue[MESSAGE_QUEUE_SIZE];
int Display::message_queue_length = 0;
long Display::message_display_start = -1;

Display::Display() : initialized(false), last_update(0ul), last_wifi_scan(0ul)
{
    manager = nullptr;
    message_queue_length = 0;
    message_display_start = -1;
}

void Display::init()
{
    if (initialized)
        return;

    // Send temporary message to display
    Message message{"Initialization...", 1};
    add_message(message);

    update_display("main.temperature.pco=" + String(DARKGREY));
    update_display("main.temperature.txt=\"--.-\"");

    update_display("main.resState.txt=\"OFF\"");
    update_display("main.pumpState.txt=\"OFF\"");

    update_display("vis 17,0");
    update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    current_page = main;
    current_ssids_page = 0;
    ssids_count = -2;

    initialized = true;
    delay(1000);
}

//* MESSAGE QUEUE *//
void Display::add_message(Message message)
{
    if (message_queue_length == MESSAGE_QUEUE_SIZE)
        return;

    message_queue[message_queue_length] = message;
    message_queue_length++;

    // if the message is the first one in the queue, display it
    if (message_queue_length == 1)
        display_messages();
}

void Display::display_messages()
{
    if (message_queue_length == 0)
        return;

    Message message = message_queue[0];

    // check if start_time is set, if it isn't it means that the message has not been displayed yet
    // We need to set the start_time and display the message
    if (message_display_start == -1)
    {
        if (message.text == "Initialization...")
            update_display("");

        message_display_start = millis();
        update_display("main.state.txt=\"" + message.text + "\"");
        update_display("main.state.pco=" + String(ORANGE));

        return;
    }

    // check if the message has been displayed for the required duration
    if (millis() - message_display_start >= message.getDuration())
    {
        // remove the message from the queue
        for (int i = 0; i < message_queue_length - 1; i++)
            message_queue[i] = message_queue[i + 1];

        message_queue_length--;
        message_display_start = -1;

        // if there are more messages in the queue, display the next one
        if (message_queue_length > 0)
            Display::display_messages();

        // if there are no more messages in the queue, display the default message
        else
            update_state();
    }
}

//* UPDATE FUNCTIONS *//
void Display::update_resistance()
{
    // No need to update resistance
    if (!this->manager->state.get_resistance_update())
        return;

    if (this->manager->state.resistance_state == HIGH)
        update_display("main.resState.txt=\"ON\"");

    else
        update_display("main.resState.txt=\"OFF\"");
}

void Display::update_pump()
{
    // No need to update pump
    if (!this->manager->state.get_pump_update())
        return;

    if (this->manager->state.pump_state == HIGH)
        update_display("main.pumpState.txt=\"ON\"");

    else
        update_display("main.pumpState.txt=\"OFF\"");
}

void Display::update_temperature()
{
    // No need to update temperature
    if (!this->manager->state.get_temperature_update())
    {
        update_display("main.temperature.pco=" + String(GREEN));
        update_display("vis 17,0");

        return;
    }

    update_display("main.temperature.txt=\"" + String(this->manager->state.current_temperature, 1) + char(176) + "\"");

    float temperatureTrend = this->manager->state.current_temperature - this->manager->state.previous_temperature;

    if (temperatureTrend > 0)
    {
        update_display("main.temperature.pco=" + String(RED));
        update_display("vis 17,1");
    }

    else if (temperatureTrend < 0)
    {
        update_display("main.temperature.pco=" + String(BLUE));
        update_display("vis 17,1");
    }
}

void Display::update_state()
{
    // Check if current cycle status is in "IDLE"
    // "IDLE" => "Warming-up"
    if (manager->get_status() == IDLE)
    {
        // Display the message "HEATING UP" on the display
        // Update the display with a RED color
        Display::update_display("main.state.txt=\"HEATING UP\"");
        Display::update_display("main.state.pco=" + String(SALMON));

        return;
    }

    // Termination message
    // @Color: GREEN
    if (!manager->is_running())
    {
        update_display("main.state.txt=\"TERMINATED!\"");
        update_display("main.state.pco=" + String(LIGHTGREEN));

        return;
    }

    String cycle = manager->cycles[manager->current_cycle].get_name();
    String term = manager->cycles[manager->current_cycle].get_term_name();

    // We update the screen with the cycle and term name
    // @color: SKYBLUE
    update_display("main.state.txt=\"" + cycle + " - " + term + "\"");
    update_display("main.state.pco=" + String(SKYBLUE));
}

void Display::update_display(String command)
{
    ScreenTX.print(command + NEXTION_END_STRING);
}

//* COMMAND FUNCTIONS *//
void Display::listen_for_commands()
{
    static bool rx = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (ScreenRX.available() > 0)
    {
        rc = ScreenRX.read();

        if (rx == true)
        {
            if (rc != endMarker)
            {
                input_buffer[ndx] = rc;
                ndx++;
                if (ndx >= INPUT_BUFFER_SIZE)
                {
                    ndx = INPUT_BUFFER_SIZE - 1;
                }
            }
            else
            {
                input_buffer[ndx] = '\0'; // terminate the string
                rx = false;
                ndx = 0;

                //? Format of command string: "C:C[CMD][VALUE]?"
                // C: Command
                // C[CMD]: Command type
                // C[VALUE]: Command value
                // "?": End of command

                if (input_buffer[0] == 'C' && input_buffer[1] == ':' && input_buffer[2] == 'C')
                {
                    String input = String(input_buffer);
                    String command = input.substring(3, 6);
                    String value = input.substring(6, input.indexOf("?"));

                    handle_command(get_command(command), value);
                }
            }
        }

        else if (rc == startMarker)
        {
            rx = true;
        }
    }
}

void Display::handle_command(Command command, String value)
{
    switch (command)
    {
    case GET:
        get_wifi_list(value);
        break;
    case PAG:
        set_page(value);
        break;
    case CON:
        connect_to_wifi(value);
        break;
    case DIS:
        disconnect_from_wifi();
        break;
    default:
        break;
    }
}

//* COMMAND HANDLERS *//
void Display::set_page(String page)
{
    current_page = get_page(page);
}

void Display::connect_to_wifi(String credentials)
{
    // Format of credentials string: "SSID:PASSWORD"
    String ssid = credentials.substring(0, credentials.indexOf(":"));
    String password = credentials.substring(credentials.indexOf(":") + 1);

    // Set status text to "Connecting..."
    update_display("wifi_creds.connecting.val=1");
    update_display("wifi_creds.status.txt=\"Connecting\"");

    // if already connected to wifi, disconnect
    if (WiFi.status() == WL_CONNECTED)
        WiFi.disconnect();

    // Set wifi icon to disconnected
    update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    //  connect to wifi
    WiFi.begin(ssid, password);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        // check if wrong password
        if (WiFi.status() == WL_CONNECT_FAILED)
        {
            update_display("wifi_creds.connecting.val=0");
            update_display("wifi_creds.status.txt=\"Wrong password.\"");

            return;
        }

        // check if connection failed for some other reason after 20 iteration (10 seconds)
        if (i++ > 20)
        {
            update_display("wifi_creds.connecting.val=0");
            update_display("wifi_creds.status.txt=\"Connection failed.\"");

            return;
        }
    }

    // Connection successful
    update_display("wifi_creds.connecting.val=0");
    update_display("wifi_creds.status.txt=\"Connected!\"");

    Message msg = Message{"Connected to WiFi", 1};
    Display::add_message(msg);

    // Set wifi icon to connected
    update_display("main.wifi.pic=4");
    update_display("main.wifi.pic2=4");

    // Set wifi credentials (saved to internal flash)
    this->manager->network.save_wifi_credentials(ssid, password);
}

void Display::disconnect_from_wifi()
{
    // Set wifi icon to disconnected
    update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    // Disconnect from wifi
    WiFi.disconnect();

    // Send disconnect message to Display
    Message msg = Message{"Disconnected from WiFi", 1};
    Display::add_message(msg);

    // Clear wifi credential variables (values are kept internal flash)
    this->manager->network.clear_wifi_credentials();
}

void Display::get_wifi_list(String value)
{
    // New scan happens under 4 conditions:
    // 1. First time the wifi list is being requested
    // 2. User on first scroll page
    // 3. The wifi list is being requested after 10 seconds have passed since the last scan
    // 4. The wifi list is being requested after the user has pressed the refresh (value = -1)

    if (((millis() - last_wifi_scan > 45000 || ssids_count == -2) && value.toInt() == 0) || value.toInt() == -1)
    {
        // Hide refresh button
        update_display("vis 30,0");
        // If there are any old networks in the list, delete them
        if (ssids_count > 0)
            WiFi.scanDelete();

        WiFi.scanNetworks(true);
        last_wifi_scan = millis();
    }

    do
    {
        display_ssid_list(value);
        yield();
    } while (ssids_count < 0);
}

void Display::display_ssid_list(String page)
{
    ssids_count = WiFi.scanComplete();
    if (ssids_count < 0) // Scan still in progress
        return;

    // Set the current page
    current_ssids_page = page.toInt() == -1 ? 0 : page.toInt();

    // check how many pages we need to display all the ssids
    int page_count = round((float)ssids_count / 4);

    // send the number of pages to the display
    // If we only have 1 page, we don't need to show the scrollbar
    if (page_count == 1)
    {
        update_display("wifi_list.scrollbar.hig=0");
        update_display("wifi_list.scrollbar.maxval=0");
    }

    else
    {

        update_display("wifi_list.scrollbar.maxval=" + String(page_count - 1));
        update_display("wifi_list.scrollbar.val=" + String(page_count - current_ssids_page - 1));
        update_display("wifi_list.scrollbar.hig=255");
    }

    // check how many items we need to display on current page if we're on the first or last page
    int items_to_display = 4;
    if (page_count == 1)
        items_to_display = ssids_count < 4 ? ssids_count : 4;

    else if (current_ssids_page == page_count - 1)
        items_to_display = ssids_count % 4;

    // if last or first page have less than 4 items, hide the rest
    if ((current_ssids_page == page_count - 1 || current_ssids_page == 0) && items_to_display < 4)
    {
        for (int i = 3; i >= items_to_display; i--)
        {
            update_display("vis item" + String(i) + ",0");
            update_display("vis ssid" + String(i) + ",0");
            update_display("vis wifiStrength" + String(i) + ",0");
            update_display("vis click" + String(i) + ",0");
        }
    }

    // render the slots visible depending on how many items we need to display
    // and display the ssids
    for (int i = 0; i < items_to_display; ++i)
    {
        update_display("vis item" + String(i) + ",1");
        update_display("vis ssid" + String(i) + ",1");
        update_display("vis wifiStrength" + String(i) + ",1");

        int ssid_index = i + current_ssids_page * 4;
        String ssid = WiFi.SSID(ssid_index);
        int strength = WiFi.RSSI(ssid_index);

        // display the ssid
        update_display("wifi_list.ssid" + String(i) + ".txt=\"" + ssid + "\"");
        update_display("vis click" + String(i) + ",1");

        // display the wifi strength (HIGH, MED,LOW)
        if (strength > -68)
            update_display("wifi_list.wifiStrength" + String(i) + ".pic=" + String(WIFI_HIGH_PIC));
        else if (strength > -79)
            update_display("wifi_list.wifiStrength" + String(i) + ".pic=" + String(WIFI_MED_PIC));
        else
            update_display("wifi_list.wifiStrength" + String(i) + ".pic=3" + String(WIFI_LOW_PIC));
    }

    // Show the refresh button
    update_display("vis 30,1");
    // Finally, we set the "updated" flag to true(1)  on the display to trigger the render of the page
    update_display("wifi_list.updated.val=1");
}

//? LOOP FUNCTION
void Display::loop()
{
    init();
    listen_for_commands();

    if (millis() - last_update < 1000)
        return;

    // Only update temperature on main page to  avoid flickering on other pages
    if (current_page == main)
        update_temperature();

    update_pump();
    update_resistance();
    display_messages();

    last_update = millis();
}