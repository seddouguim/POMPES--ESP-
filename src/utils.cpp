#include "utils.h"

Command get_command(String command)
{
    if (command == "GET")
        return GET;
    if (command == "PAG")
        return PAG;

    if (command == "CON")
        return CON;

    if (command == "DIS")
        return DIS;

    return UNKNOWN_COMMAND;
}

Page get_page(String page)
{
    if (page == "MAIN")
        return main;

    if (page == "WIFI_LIST")
        return WIFI_LIST;

    if (page == "WIFI_CREDS")
        return WIFI_CREDS;

    if (page == "SETTINGS")
        return SETTINGS;

    return UNKNOWN_PAGE;
}