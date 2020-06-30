/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>

#include "settings.h"
#include "monitor.h"
#include "logger.h"


static struct {
    int temp_low;
    int temp_high;
    int temp_max;
    int time_poll;
    int daemon;
    int verbose;
} settings = {
    .temp_low = 63,
    .temp_high = 66,
    .temp_max = 84,
    .time_poll = 1,
    .daemon = 0,
    .verbose = 0
};


int settings_get_value(int setting) {
    switch (setting) {
        case SET_TEMP_LOW:
            return settings.temp_low;
        case SET_TEMP_HIGH:
            return settings.temp_high;
        case SET_TEMP_MAX:
            return settings.temp_max;
        case SET_TIME_POLL:
            return settings.time_poll;
        case SET_DAEMON:
            return settings.daemon;
        case SET_VERBOSE:
            return settings.verbose;
        default:
            return -1;
    }
}


int settings_set_value(int setting, int value) {
    if (value < 0 || setting < SET_TEMP_LOW || setting > SET_VERBOSE)
        return 0;
    switch (setting) {
        case SET_TEMP_LOW:
            settings.temp_low = value;
            break;
        case SET_TEMP_HIGH:
            settings.temp_high = value;
            break;
        case SET_TEMP_MAX:
            settings.temp_max = value;
            break;
        case SET_TIME_POLL:
            settings.time_poll = value;
            break;
        case SET_DAEMON:
            settings.daemon = value;
            break;
        case SET_VERBOSE:
            settings.verbose = value;
            break;
    }
    return 1;
}