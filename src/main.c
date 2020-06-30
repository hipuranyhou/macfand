/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <syslog.h>
#include <errno.h>

#include "fan.h"
#include "monitor.h"
#include "helper.h"
#include "settings.h"
#include "control.h"
#include "daemonize.h"
#include "linked.h"
#include "logger.h"


const char *argp_program_version = "macfand - version 0.1";


static struct argp_option options[] = { 
    {"daemon", 'd', 0, 0, "Run in daemon mode"},
    {"poll", 'p', "NUM", 0, "Set poll time in seconds (whole number bigger than 0)"},
    {"low", 'l', "NUM", 0, "Set temperature under which fans run on min speed (whole number bigger than 0)"},
    {"high", 'h', "NUM", 0, "Set temperature used for determining the aggresivity of speed adjustment (whole number bigger than 29"},
    {"verbose", 'v', 0, 0, "Enables verbose mode. Not allowed in daemon mode!"},
    {0}
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) { 
    t_settings *settings = state->input;
    switch(key) {
        case 'd':
            settings->daemon = 1;
            break;
        case 'p':
            if (!get_int_from_string(arg, &(settings->time_poll)) || settings->time_poll < 1)
                argp_failure(state, 1, 0, "Poll time is invalid!");
            break;
        case 'l':
            if (!get_int_from_string(arg, &(settings->temp_low)) || settings->temp_low < 1)
                argp_failure(state, 1, 0, "Low temp is invalid!");
            break;
        case 'h':
            if (!get_int_from_string(arg, &(settings->temp_high)) || settings->temp_high < 30)
                argp_failure(state, 1, 0, "High temp is invalid!");
            break;
        case 'v':
            settings->verbose = 1;
            break;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, 0, 0};


int main(int argc, char **argv) {
    t_settings settings;
    t_node *monitors = NULL, *fans = NULL;

    settings_load_defaults(&settings);

    logger_set_type(LOG_T_SYS, NULL);

    // Argp leaking memory on failure?
    argp_parse(&argp, argc, argv, 0, 0, &settings);

    if (settings.daemon)
        daemonize();

    monitors = monitors_load();
    if (!monitors) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load system temperature monitors");
        return 1;
    }

    settings_set_max_temp(&settings, monitors);

    fans = fans_load(&settings);
    if (!fans) {
        list_free(monitors, (void (*)(void *))monitor_free);
        logger_log(LOG_L_ERROR, "%s", "Unable to load system fans");
        return 1;
    }

    if (!fans_set_mode(fans, FAN_MANUAL)) {
        // Set fans back to auto if enabling manual mode failed 
        // Those we were unable to set to manual mode are already in automatic mode
        fans_set_mode(fans, FAN_AUTO);
        list_free(monitors, (void (*)(void *))monitor_free);
        list_free(fans, (void (*)(void *))fan_free);
        logger_log(LOG_L_ERROR, "%s", "Unable to set fans to manual mode");
        return 1;
    }

    control_start(&settings, fans, monitors);

    if (!fans_set_mode(fans, FAN_AUTO))
        logger_log(LOG_L_ERROR, "%s", "Unable to reset fans to automatic mode");

    list_free(monitors, (void (*)(void *))monitor_free);
    list_free(fans, (void (*)(void *))fan_free);

    logger_exit();

    return 0;
}
