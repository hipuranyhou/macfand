/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <argp.h>
#include <signal.h>
#include <string.h>

#include "init.h"
#include "linked.h"
#include "monitor.h"
#include "fan.h"
#include "logger.h"
#include "settings.h"
#include "daemonize.h"
#include "control.h"
#include "config.h"

/**
 * @brief Struct used for argp.
 * Struct used for returning command line arguments from argp to main.
 */
struct args {
    int no_conf;
};

/**
 * @brief Prepares settings for macfand.
 * Prepares settings for macfand by reading configuration file if it is not disabled, sets
 * default paths (log file, i3 widget, ...) and checks validity of settings.
 * @param[in] args Pointer to struct of argp arguments.
 * @return int 0 on error, 1 on success.
 */
static int init_set(const struct args *const args);

/**
 * @brief Sets the termination flag.
 * Sets the termination flag to sig for exiting out of main control loop when termination signal is catched.
 * @param[in] sig Catched signal number.
 */
static void set_term_flag(int sig);

/**
 * @brief Sets the reload config flag.
 * Sets the reload config flag to sig for reloading config in main control loop when SIGHUP is catched.
 * @param[in] sig Catched signal number.
 */
static void set_rld_flag(int sig);

/**
 * @brief Wrapper for all sigaction() calls.
 * Wrapper for all sigaction() calls for all signals we want to register.
 * @return int 0 on error, 1 on success.
 */
static int init_sig(void);

/**
 * @brief Prepares generic linked lists of monitors and fans for use.
 * Used to load generic linked lists of monitors and fans for use. Loads max temperature 
 * into settings and sets fans to automatic mode. Logs error messages and both lists using logger.
 * @param[in,out] mons Pointer to head of linked list of temperature monitors.
 * @param[in,out] fans Pointer to head of linked list of system fans.
 * @return int 0 on error, 1 on success
 */
static int init_mons_fans(t_node **mons, t_node **fans);

/**
 * @brief Frees all allocated memory and logs exit.
 * Used in main() before exit. Frees memory used by lists mons and fans, resets fans to 
 * automatic mode, frees string settings and prepares logger for exit.
 * @param[in,out] mons Pointer to head of linked list of temperature monitors.
 * @param[in,out] fans Pointer to head of linked list of system fans.
 */
static void init_exit(t_node *mons, t_node *fans);


static int init_set(const struct args *const args) {
    if (!args->no_conf) {
        // Load config
        if (!conf_load(set_get_str(SET_CONFIG_FILE_PATH))) {
            log_log(LOG_L_ERROR, "Unable to load configuration file.");
            return 0;
        }
        log_log(LOG_L_INFO, "Using configuration file %s", set_get_str(SET_CONFIG_FILE_PATH));
    } else
        log_log(LOG_L_INFO, "Using default settings without configuration file");

    // Load default paths and check validity of settings
    if (!set_check()) {
        log_log(LOG_L_ERROR, "Settings are invalid");
        return 0;
    }

    return 1;
}


extern volatile sig_atomic_t term_flag;
extern volatile sig_atomic_t rld_flag;


static void set_term_flag(int sig) {
    term_flag = sig;
}


static void set_rld_flag(int sig) {
    rld_flag = sig;
}


static int init_sig(void) {
    struct sigaction action;

    action.sa_flags = 0;
    if (sigemptyset(&action.sa_mask) < 0)
        return 0;

    // Prepare all signals after which we terminate
    action.sa_handler = set_term_flag;
    if (sigaction(SIGABRT, &action, NULL) < 0 ||
        sigaction(SIGINT, &action, NULL) < 0  ||
        sigaction(SIGQUIT, &action, NULL) < 0 ||
        sigaction(SIGTERM, &action, NULL) < 0)
        return 0;

    // Reload config action
    action.sa_handler = set_rld_flag;
    if (sigaction(SIGHUP, &action, NULL) < 0)
        return 0;

    return 1;
}


static int init_mons_fans(t_node **mons, t_node **fans) {
    // Temperature monitors
    *mons = mons_load();
    if (!(*mons)) {
        log_log(LOG_L_ERROR, "Unable to load system temperature monitors");
        return 0;
    }
    if (!set_set_int(SET_TEMP_MAX, mons_read_temp_max(*mons))) {
        log_log(LOG_L_ERROR, "Unable to load max temperature");
        return 0;
    }
    if (set_get_int(SET_VERBOSE))
        log_log_list("monitors", *mons, (void (*)(const void *, FILE *const))mon_print);

    // Fans
    *fans = fans_load();
    if (!(*fans)) {
        log_log(LOG_L_ERROR, "Unable to load system fans");
        return 0;
    }
    if (set_get_int(SET_VERBOSE))
        log_log_list("fans", *fans, (void (*)(const void *, FILE *const))fan_print);
    if (!fans_write_mod(*fans, FAN_M_MAN)) {
        log_log(LOG_L_ERROR, "Unable to set fans to manual mode");
        return 0;
    }

    return 1;
}


void init_exit(t_node *mons, t_node *fans) {
    if (mons)
        list_free(mons, (void (*)(void *, int))mon_free);

    if (fans) {
        if (!fans_write_mod(fans, FAN_M_AUTO))
            log_log(LOG_L_ERROR, "Unable to reset fans to automatic mode.");
        list_free(fans, (void (*)(void *, int))fan_free);
    }

    set_free();
    log_exit();
}

/************************ arpg stuff ************************/

const char *argp_program_version = "macfand - version 0.1-alpha";


static struct argp_option options[] = { 
    {"config", 'c', "PATH", 0, "Path to custom configuration file (no, false or 0 to disable check for config file)", 1},
    {"verbose", 'v', NULL, 0, "Enable verbose mode", 2},
    {0}
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) { 
    struct args *args = state->input;

    switch(key) {
        case 'c':
            if (strcmp(arg, "no") == 0 || strcmp(arg, "false") == 0 || strcmp(arg, "0") == 0) {
                args->no_conf = 1;
                break;
            }
            if (!set_set_str(SET_CONFIG_FILE_PATH, arg))
                argp_failure(state, 1, 0, "Unknown error encoutered while setting config file path.");
            break;
        case 'v':
            set_set_int(SET_VERBOSE, 1);
            break;
    }

    return 0;
}


static struct argp argp = {options, parse_opt, NULL, NULL, NULL, NULL, NULL};

/************************************************************/


int init_load(int argc, char **argv) {
    t_node      *mons  = NULL;
    t_node      *fans  = NULL;
    struct args args   = {
        .no_conf = 0,
    };

    // Argp leaking memory on failure?
    argp_parse(&argp, argc, argv, 0, NULL, &args);

    // Load settings
    if (!init_set(&args)) {
        init_exit(mons, fans);
        return 0;
    }

    if (!init_sig()) {
        log_log(LOG_L_ERROR, "Unable to register signal handlers.");
        init_exit(mons, fans);
        return 0;
    }

    // Daemonize
    if (set_get_int(SET_DAEMON))
        daemonize();

    // Set logger to configured type
    if (!log_set_type(set_get_int(SET_LOG_TYPE), set_get_str(SET_LOG_FILE_PATH))) {
        log_log(LOG_L_ERROR, "Unable to set logger to configured mode");
        init_exit(mons, fans);
        return 0;
    }

    // Load system temperature monitors a fans
    if (!init_mons_fans(&mons, &fans)) {
        init_exit(mons, fans);
        return 0;
    }

    // Start main control loop
    if (!ctrl_start(mons, fans)) {
        log_log(LOG_L_ERROR, "Main control loop failed.");
        init_exit(mons, fans);
        return 0;
    }

    init_exit(mons, fans);
    return 1;
}