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
    {"poll", 'p', "NUM", 0, "Sets poll time in seconds (whole number >= 1)"},
    {"low", 'l', "NUM", 0, "Sets temperature under which fans run on min speed (whole number >= 1)"},
    {"high", 'h', "NUM", 0, "Sets temperature used for determining the aggresivity of speed adjustment (whole number >= 30)"},
    {"verbose", 'v', 0, 0, "Enables verbose mode."},
    {"type", 't', "NUM/PATH", 0, "Sets type of logger used (0 -> std, 1 -> sys, other values taken as log file path)"},
    {0}
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) { 
    int tmp = 0;
    switch(key) {
        case 'd':
            settings_set_value(SET_DAEMON, 1);
            break;
        case 'p':
            if (!get_int_from_string(arg, &tmp) || tmp < 1)
                argp_failure(state, 1, 0, "Poll time is invalid");
            settings_set_value(SET_TIME_POLL, 1);
            break;
        case 'l':
            if (!get_int_from_string(arg, &tmp) || tmp < 1)
                argp_failure(state, 1, 0, "Low temp is invalid");
            settings_set_value(SET_TEMP_LOW, 1);
            break;
        case 'h':
            if (!get_int_from_string(arg, &tmp) || tmp < 30)
                argp_failure(state, 1, 0, "High temp is invalid");
            settings_set_value(SET_TEMP_HIGH, 1);
            break;
        case 'v':
            settings_set_value(SET_VERBOSE, 1);
            break;
        case 't':
            if (!get_int_from_string(arg, &tmp))
                tmp = -1;
            switch (tmp) {
                case 0:
                    logger_set_type(LOG_T_STD, NULL);
                    break;
                case 1:
                    logger_set_type(LOG_T_SYS, NULL);
                    break;
                default:
                    if (!logger_set_type(LOG_T_FILE, arg))
                        argp_failure(state, 1, 0, "Unable to open log file");
                    break;
            }
            break;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, 0, 0};


int main(int argc, char **argv) {
    t_node *monitors = NULL, *fans = NULL;

    // Argp leaking memory on failure?
    argp_parse(&argp, argc, argv, 0, 0, 0);

    if (settings_get_value(SET_DAEMON))
        daemonize();

    monitors = monitors_load();
    if (!monitors) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load system temperature monitors");
        return 1;
    }

    // We do this twice for fans and monitors to get at least monitors printed if loading fans fails
    if (settings_get_value(SET_VERBOSE))
        logger_log_list(monitors, (void (*)(const void *, FILE *))monitor_print);

    if (!settings_set_value(SET_TEMP_MAX, monitors_get_max_temp(monitors)))
        logger_log(LOG_L_WARN, "%s", "Using default max temperature value 84");

    fans = fans_load();
    if (!fans) {
        list_free(monitors, (void (*)(void *))monitor_free);
        logger_log(LOG_L_ERROR, "%s", "Unable to load system fans");
        return 1;
    }

    // We do this twice for fans and monitors to get at least monitors printed if loading fans fails
    if (settings_get_value(SET_VERBOSE))
        logger_log_list(fans, (void (*)(const void *, FILE *))fan_print);

    if (!fans_set_mode(fans, FAN_MANUAL)) {
        // Set fans back to auto if enabling manual mode failed 
        // Those we were unable to set to manual mode are already in automatic mode
        fans_set_mode(fans, FAN_AUTO);
        list_free(monitors, (void (*)(void *))monitor_free);
        list_free(fans, (void (*)(void *))fan_free);
        logger_log(LOG_L_ERROR, "%s", "Unable to set fans to manual mode");
        return 1;
    }

    control_start(fans, monitors);

    // Procedure after termination signal catched
    list_free(monitors, (void (*)(void *))monitor_free);

    if (!fans_set_mode(fans, FAN_AUTO)) {
        logger_log(LOG_L_ERROR, "%s", "Unable to reset fans to automatic mode");
        list_free(fans, (void (*)(void *))fan_free);
        logger_exit();
        return 1;
    }

    list_free(fans, (void (*)(void *))fan_free);
    logger_exit();

    return 0;
}
