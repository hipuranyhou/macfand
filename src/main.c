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
#include <string.h>

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
 * @brief Struct used for argp.
 * Struct used for returning command line arguments from argp to main.
 */
typedef struct arguments {
    int no_config;
    char *config_file_path;
} arguments_t;

/**
 * @brief Frees all allocated memory and logs exit.
 * Used in main() before exit. Frees memory used by lists monitors and fans,
 * string settings and prepares logger for exit.
 * @param[in]  fans      Pointer to head of linked list of system fans.
 * @param[in]  monitors  Pointer to head of linked list of temperature monitors.
 */
static void prepare_exit(t_node *monitors, t_node *fans);

/**
 * @brief Prepares generic linked lists of monitors and fans for use.
 * Used to load generic linked lists of monitors and fans for use. Loads max temperature 
 * into settings and sets fans to automatic mode. Logs error messages and both lists using logger.
 * @param[in,out]  fans      Pointer to head of linked list of system fans.
 * @param[in,out]  monitors  Pointer to head of linked list of temperature monitors.
 * @return int 0 on error, 1 on success
 */
static int prepare_monitors_and_fans(t_node **monitors, t_node **fans);

/**
 * @brief Prepares settings for macfand.
 * Prepares settings for macfand by reading configuration file if it is not disabled, sets
 * default paths (log file, i3 widget, ...) and checks validity of settings.
 * @param[in]  arguments  Pointer to struct of argp arguments.
 * @return int 0 on error, 1 on success.
 */
int prepare_settings(const arguments_t *arguments);


/*******************************  argp stuff  ***********************************/

const char *argp_program_version = "macfand - version 0.1";


static struct argp_option options[] = { 
    {"config", 'c', "PATH", 0, "Path to custom configuration file (no, false or 0 to disable check for config file)", 1},
    {"verbose", 'v', NULL, 0, "Enable verbose mode", 2},
    {0}
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) { 
    arguments_t *arguments = state->input;
    switch(key) {
        case 'c':
            if (strcmp(arg, "no") == 0 || strcmp(arg, "false") == 0 || strcmp(arg, "0") == 0) {
                arguments->no_config = 1;
                break;
            }
            arguments->config_file_path = arg;
            break;
        case 'v':
            settings_set_value(SET_VERBOSE, 1);
            break;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, NULL, NULL, NULL, NULL, NULL};

/******************************************************************************/


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
        return 1;
    }   

    if (!settings_set_value(SET_TEMP_MAX, monitors_get_temp_max(*monitors))) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load max temperature");
        return 0;
    }

    if (settings_get_value(SET_VERBOSE))
        logger_log_list("monitors", *monitors, (void (*)(const void *, FILE *))monitor_print);

    if (!(*fans = fans_load())) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load system fans");
        return 0;
    }

    if (settings_get_value(SET_VERBOSE))
        logger_log_list("fans", *fans, (void (*)(const void *, FILE *))fan_print);

    if (!fans_set_mode(*fans, FAN_MANUAL)) {
        // Those we were unable to set to manual mode are already in automatic mode
        fans_set_mode(*fans, FAN_AUTO);
        logger_log(LOG_L_ERROR, "%s", "Unable to set fans to manual mode");
        return 0;
    }

    return 1;
}


int prepare_settings(const arguments_t *arguments) {

    if (!arguments->no_config) {
        // Load config
        if (!config_load((arguments->config_file_path) ? arguments->config_file_path : "/etc/macfand.config")) {
            logger_log(LOG_L_ERROR, "%s", "Unable to load configuration file");
            return 0;
        }
    }

    if (arguments->no_config)
        logger_log(LOG_L_WARN, "%s", "Using default settings");

    // Load default paths and check validity of settings
    if (!settings_check()) {
        logger_log(LOG_L_ERROR, "%s", "Settings are invalid");
        return 0;
    }

    return 1;
}


int main(int argc, char **argv) {
    t_node *monitors = NULL, *fans = NULL;
    arguments_t arguments = {
        .no_config = 0,
        .config_file_path = NULL
    };

    // Argp leaking memory on failure?
    argp_parse(&argp, argc, argv, 0, NULL, &arguments);

    // Load settings
    if (!prepare_settings(&arguments)) {
        prepare_exit(monitors, fans);
        return EXIT_FAILURE;
    }

    // Load system temperature monitors a fans
    if (!prepare_monitors_and_fans(&monitors, &fans)) {
        prepare_exit(monitors, fans);
        return EXIT_FAILURE;
    }

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
