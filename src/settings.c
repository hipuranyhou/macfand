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

void settings_load_defaults(t_settings *settings) {
    settings->low_temp = 63;
    settings->high_temp = 66;
    settings->max_temp = 84;
    settings->poll_time = 1;
    settings->daemon = 0;
    settings->verbose = 0;
}

void settings_set_max_temp(t_settings *settings, const t_node *monitors) {
    t_monitor *monitor = NULL;
    while (monitors) {
        monitor = monitors->data;
        settings->max_temp = (settings->max_temp > monitor->temp_max) ? settings->max_temp : monitor->temp_max;
        monitors = monitors->next;
    }
}