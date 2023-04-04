#include "Display.h"
#include "Manager.h"

Display::Display() : initialized(false), last_update(0ul), last_wifi_scan(0ul)
{
    manager = nullptr;
}

// declare the static variable
Manager *Display::manager = nullptr;

void Display::init()
{
    if (initialized)
        return;

    update_display("main.state.pco=" + String(WHITE));
    update_display("main.state.txt=\"Initialization...\"");
    update_display("main.resState.txt=\"OFF\"");
    update_display("main.pumpState.txt=\"OFF\"");
    update_display("main.temperature.pco=" + String(DARKGREY));
    update_display("main.temperature.txt=\"--.-\"");

    update_display("vis 17,0");
    update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    current_page = main;
    current_ssids_page = 0;
    ssids_count = -2;

    initialized = true;
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
    if (manager->get_status() == UNKNOWN_STATUS)
        return;

    // Check if current cycle status is not in "IDLE"
    // "IDLE" => "Warming-up"
    if (manager->get_status() == IDLE)
    {
        // Display the message "HEATING UP" on the display
        // Update the display with a RED color
        Display::update_display("main.state.pco=" + String(SALMON));
        Display::update_display("main.state.txt=\"HEATING UP\"");

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
                    Serial.println("Input: " + input);
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
    update_display("wifi_creds.status.txt=\"Connecting\"");
    update_display("wifi_creds.connecting.val=1");

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
            // Set status text to "Wrong password."
            update_display("wifi_creds.connecting.val=0");
            update_display("wifi_creds.status.txt=\"Wrong password.\"");

            return;
        }

        // check if connection failed for some other reason after 10 iteration (20 seconds)
        if (i++ > 20)
        {
            // Set status text to "Connection failed."
            update_display("wifi_creds.connecting.val=0");
            update_display("wifi_creds.status.txt=\"Connection failed.\"");

            return;
        }
    }

    // Connection successful
    // Set status text to "Connected!"
    update_display("wifi_creds.connecting.val=0");
    update_display("wifi_creds.status.txt=\"Connected!\"");

    // Set wifi icon to connected
    update_display("main.wifi.pic=4");
    update_display("main.wifi.pic2=4");

    // Set wifi credentials (saved to internal flash)
    this->manager->network.save_wifi_credentials(ssid, password);
}

void Display::disconnect_from_wifi()
{
    Serial.println("Disconnecting from wifi");
    // Set wifi icon to disconnected
    update_display("main.wifi.pic=" + String(WIFI_DISCONNECTED_PIC));

    // Disconnect from wifi
    WiFi.disconnect();

    // Clear wifi credential variables (keep the values in internal flash)
    this->manager->network.clear_wifi_credentials();
}

void Display::get_wifi_list(String value)
{
    // New scan happens under 4 conditions:
    // 1. First time the wifi list is being requested
    // 2. User on first scroll page
    // 3. The wifi list is being requested after 10 seconds have passed since the last scan
    // 4. The wifi list is being requested after the user has pressed the refresh

    if ((millis() - last_wifi_scan > 45000 || ssids_count == -2) && value.toInt() == 0)
    {
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
    current_ssids_page = page.toInt();

    // check how many pages we need to display all the ssids
    int page_count = round((float)ssids_count / 4);

    // send the number of pages to the display
    // If we only have 1 page, we don't need to show the scrollbar
    if (page_count == 1)
    {
        update_display("wifi_list.scrollbar.wid=0");
        update_display("wifi_list.scrollbar.maxval=0");
    }

    else
    {
        update_display("wifi_list.scrollbar.wid=255");
        update_display("wifi_list.scrollbar.maxval=" + String(page_count - 1) + NEXTION_END_STRING);
    }

    // check how many items we need to display on current page if we're on the first or last page
    int items_to_display = 4;
    if (page_count == 1)
        items_to_display = ssids_count < 4 ? ssids_count : 4;

    else if (current_ssids_page == page_count - 1)
        items_to_display = ssids_count % 4;

    // if last page has less than 4 items, hide the rest
    if (current_ssids_page == page_count - 1)
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

    last_update = millis();
}