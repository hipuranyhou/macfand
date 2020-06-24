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

#include "fan.h"
#include "monitor.h"
#include "helper.h"
#include "settings.h"
#include "control.h"
#include "daemonize.h"
#include "linked.h"


const char *argp_program_version = "macfand - version 0.1";


static struct argp_option options[] = { 
    {"daemon", 'd', 0, 0, "Run in daemon mode"},
    {"poll", 'p', "NUM", 0, "Set poll time in seconds (whole number bigger than 0)"},
    {"low", 'l', "NUM", 0, "Set temperature under which fans run on min speed (whole number 55 < NUM)"},
    {"high", 'h', "NUM", 0, "Set temperature used for determining the aggresivity of speed adjustment (whole number 55 < NUM)"},
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
            if (atoi(arg) < 1)
                argp_failure(state, 1, 0, "Poll time must be a whole number bigger than 0");
            settings->poll_time = atoi(arg);
            break;
        case 'l':
            if (atoi(arg) < 56)
                argp_failure(state, 1, 0, "Low temp must be bigger than 55");
            settings->low_temp = atoi(arg);
            break;
        case 'h':
            if (atoi(arg) < 56)
                argp_failure(state, 1, 0, "High temp must be bigger than 55");
            settings->high_temp = atoi(arg);
            break;
        case 'v':
            settings->verbose = 1;
            break;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, 0, 0};


int main(int argc, char **argv) {
    // Load default settings
    t_settings settings;
    settings_load_defaults(&settings);

    // Parse command line arguments
    argp_parse(&argp, argc, argv, 0, 0, &settings);

    if (settings.daemon)
        daemonize();

    // Load temperature monitors and max temperature
    t_node *monitors = monitors_load();
    if (monitors == NULL) {
        fprintf(stderr, "%s\n", "Error encountered while loading temperature monitors!");
        return 1;
    }

    settings_set_max_temp(&settings, monitors);

    // Load fans
    t_node *fans = fans_load(&settings);
    if (fans == NULL) {
        list_free(monitors, (void (*)(void *))monitor_free);
        fprintf(stderr, "%s\n", "Error encountered while loading fans!");
        return 1;
    }

    // Set fans to manual mode
    if (!fans_set_mode(fans, FAN_MANUAL)) {
        // Set fans back to auto if enabling manual mode failed 
        // (those we were unable to set to manual mode are already in automatic mode)
        fans_set_mode(fans, FAN_AUTO);
        list_free(monitors, (void (*)(void *))monitor_free);
        list_free(fans, (void (*)(void *))fan_free);
        fprintf(stderr, "%s\n", "Error encountered while setting fans to manual mode!");
        return 1;
    }

    // Start main control loop
    start_control(&settings, &fans, monitors);

    // Reset fans to automatic mode when exiting
    if (!fans_set_mode(fans, FAN_AUTO)) {
        switch (settings.daemon) {
            case 0:
                fprintf(stderr, "Error while resetting fans to automatic mode.");
                break;
            case 1:
                syslog(LOG_ERR, "Error while resetting fans to automatic mode.");
                break;
        }
    }

    // Free memory and exit
    list_free(monitors, (void (*)(void *))monitor_free);
    list_free(fans, (void (*)(void *))fan_free);

    return 0;
}
