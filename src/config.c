/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com
 */

#include <stdio.h>

#include "config.h"

void load_default_settings(t_settings *settings) {
    settings->low_temp = 63;
    settings->high_temp = 66;
    settings->max_temp = 84;
    settings->poll_time = 1;
    settings->daemon = 0;
}