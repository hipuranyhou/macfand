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
#include "config.h"

/**
 * @brief 
 * 
 * @param monitors 
 * @param fans 
 */
static void prepare_exit(t_node *monitors, t_node *fans);

/**
 * @brief 
 * 
 * @param monitors 
 * @param fans 
 * @return int 
 */
static int prepare_monitors_and_fans(t_node **monitors, t_node **fans);

/**
 * @brief 
 * 
 */
typedef struct arguments {
    char *config_file_path;
} arguments_t;


const char *argp_program_version = "macfand - version 0.1";


static struct argp_option options[] = { 
    {"config", 'c', "PATH", 0, "Path to custom configuration file", 1},
    {"verbose", 'v', NULL, 0, "Enable verbose mode", 2},
    {0}
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) { 
    arguments_t *arguments = state->input;
    switch(key) {
        case 'c':
            arguments->config_file_path = arg;
            break;
        case 'v':
            settings_set_value(SET_VERBOSE, 1);
            break;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, NULL, NULL, NULL, NULL, NULL};


static void prepare_exit(t_node *monitors, t_node *fans) {
    if (monitors)
        list_free(monitors, (void (*)(void *))monitor_free);
    if (fans)
        list_free(fans, (void (*)(void *))fan_free);
    settings_free();
    logger_exit();
}


static int prepare_monitors_and_fans(t_node **monitors, t_node **fans) {

    if (!(*monitors = monitors_load())) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load system temperature monitors");
        logger_exit();
        return 1;
    }   

    if (!settings_set_value(SET_TEMP_MAX, monitors_get_temp_max(*monitors))) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load max temperature");
        prepare_exit(*monitors, *fans);
        return 0;
    }

    if (settings_get_value(SET_VERBOSE))
        logger_log_list("monitors", *monitors, (void (*)(const void *, FILE *))monitor_print);

    if (!(*fans = fans_load())) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load system fans");
        prepare_exit(*monitors, *fans);
        return 0;
    }

    if (settings_get_value(SET_VERBOSE))
        logger_log_list("fans", *fans, (void (*)(const void *, FILE *))fan_print);

    if (!fans_set_mode(*fans, FAN_MANUAL)) {
        // Those we were unable to set to manual mode are already in automatic mode
        fans_set_mode(*fans, FAN_AUTO);
        logger_log(LOG_L_ERROR, "%s", "Unable to set fans to manual mode");
        prepare_exit(*monitors, *fans);
        return 0;
    }

    return 1;
}


int main(int argc, char **argv) {
    t_node *monitors = NULL, *fans = NULL;
    arguments_t arguments = {
        .config_file_path = NULL
    };

    // Argp leaking memory on failure?
    argp_parse(&argp, argc, argv, 0, NULL, &arguments);

    // Load config
    if (!config_load((arguments.config_file_path) ? arguments.config_file_path : "/etc/macfand.config")) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load configuration file");
        logger_exit();
        return EXIT_FAILURE;
    }

    // Check validity of settings
    if (!settings_check()) {
        logger_log(LOG_L_ERROR, "%s", "Settings are invalid");
        logger_exit();
        return EXIT_FAILURE;
    }

    // Load system temperature monitors a fans
    if (!prepare_monitors_and_fans(&monitors, &fans))
        return EXIT_FAILURE;

    // Daemonize
    if (settings_get_value(SET_DAEMON))
        daemonize();

    // Set logger to configure type
    if (!logger_set_type(settings_get_value(SET_LOG_TYPE), settings_get_value_string(SET_LOG_FILE_PATH))) {
        logger_log(LOG_L_ERROR, "%s", "Unable to set logger to configured mode");
        prepare_exit(monitors, fans);
        return EXIT_FAILURE;
    }

    // Start main control loop
    control_start(monitors, fans);

    // Reset fans back to automatic mode
    if (!fans_set_mode(fans, FAN_AUTO)) {
        logger_log(LOG_L_ERROR, "%s", "Unable to reset fans to automatic mode");
        prepare_exit(monitors, fans);
        return EXIT_FAILURE;
    }

    // Exit
    prepare_exit(monitors, fans);
    return EXIT_SUCCESS;
}
