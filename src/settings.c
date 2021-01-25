/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <string.h>

#include "settings.h"
#include "logger.h"

/**
 * @brief Struct holding all settings.
 * Struct holding all settings with set defaults.
 */
static struct {
    int temp_low;
    int temp_high;
    int temp_max;
    int time_poll;
    int daemon;
    int verbose;
    int log_type;
    char *log_file_path;
    int widget;
    char *widget_file_path;
    char *config_file_path;
} set = {
    .temp_low = 63,
    .temp_high = 66,
    .temp_max = 84,
    .time_poll = 1,
    .daemon = 0,
    .verbose = 0,
    .log_type = LOG_T_STD,
    .log_file_path = NULL,
    .widget = 0,
    .widget_file_path = NULL,
    .config_file_path = NULL
};


void set_free() {
    if (set.log_file_path)
        free(set.log_file_path);
    if (set.widget_file_path)
        free(set.widget_file_path);
    if (set.config_file_path)
        free(set.config_file_path);
}


int set_check() {

    if (set.temp_low < 1) {
        log_log(LOG_L_DEBUG, "%s", "Value of temp_low must be >= 1");
        return 0;
    }
    if (set.temp_high <= set.temp_low) {
        log_log(LOG_L_DEBUG, "%s", "Value of temp_high is invalid (must be > temp_low)");
        return 0;
    }
    if (set.temp_max <= set.temp_high) {
        log_log(LOG_L_DEBUG, "%s", "Value of temp_max is invalid (must be > temp_high");
        return 0;
    }
    if (set.time_poll < 1) {
        log_log(LOG_L_DEBUG, "%s", "Value of time_poll must be >= 1");
        return 0;
    }
    if (set.daemon != 0 && set.daemon != 1) {
        log_log(LOG_L_DEBUG, "%s", "Value of daemon must be 0 or 1");
        return 0;
    }
    if (set.verbose != 0 && set.verbose != 1) {
        log_log(LOG_L_DEBUG, "%s", "Value of verbose must be 0 or 1");
        return 0;
    }
    if (set.log_type < LOG_T_STD || set.log_type > LOG_T_FILE) {
        log_log(LOG_L_DEBUG, "%s", "Value of log_type must be one of std, sys and file");
        return 0;
    }
    if (set.log_type == LOG_T_FILE && !set.log_file_path) {
        if (!set_set_val_str(SET_LOG_FILE_PATH, "/var/log/macfand.log")) {
            log_log(LOG_L_DEBUG, "%s", "Unable to set default log file path to /var/log/macfand.log");
            return 0;
        }
        log_log(LOG_L_INFO, "%s", "Using default log file path /var/log/macfand.log");
    }
    if (set.widget != 0 && set.widget != 1) {
        log_log(LOG_L_DEBUG, "%s", "Value of widget must be 0 or 1");
        return 0;
    }
    if (set.widget && !set.widget_file_path) {
        if (!set_set_val_str(SET_WIDGET_FILE_PATH, "/tmp/macfand.widget")) {
            log_log(LOG_L_DEBUG, "%s", "Unable to set default widget file path to /tmp/macfand.widget");
            return 0;
        }
        log_log(LOG_L_INFO, "%s", "Using default widget file path /tmp/macfand.widget");
    }

    return 1;
}


int set_get_val(int choice) {
    switch (choice) {
        case SET_TEMP_LOW:
            return set.temp_low;
        case SET_TEMP_HIGH:
            return set.temp_high;
        case SET_TEMP_MAX:
            return set.temp_max;
        case SET_TIME_POLL:
            return set.time_poll;
        case SET_DAEMON:
            return set.daemon;
        case SET_VERBOSE:
            return set.verbose;
        case SET_LOG_TYPE:
            return set.log_type;
        case SET_WIDGET:
            return set.widget;
        default:
            return -1;
    }
}


char* set_get_val_str(int choice) {
    switch (choice) {
        case SET_LOG_FILE_PATH:
            return set.log_file_path;
        case SET_WIDGET_FILE_PATH:
            return set.widget_file_path;
        case SET_CONFIG_FILE_PATH:
            return set.config_file_path;
        default:
            return NULL;
    }
}


int set_set_val(int choice, int val) {

    switch (choice) {
        case SET_TEMP_LOW:
            set.temp_low = val;
            break;
        case SET_TEMP_HIGH:
            set.temp_high = val;
            break;
        case SET_TEMP_MAX:
            set.temp_max = val;
            break;
        case SET_TIME_POLL:
            set.time_poll = val;
            break;
        case SET_DAEMON:
            set.daemon = val;
            break;
        case SET_VERBOSE:
            set.verbose = val;
            break;
        case SET_LOG_TYPE:
            set.log_type = val;
            break;
        case SET_WIDGET:
            set.widget = val;
            break;
        default:
            return 0;
    }

    return 1;
}


int set_set_val_str(int choice, const char *const val) { 
    if (!val)
        return 0;

    switch (choice) {

        case SET_LOG_FILE_PATH:
            if (set.log_file_path)
                free(set.log_file_path);
            set.log_file_path = (char*)malloc(strlen(val)+1);
            if (!set.log_file_path)
                return 0;
            strcpy(set.log_file_path, val);
            break;

        case SET_WIDGET_FILE_PATH:
            if (set.widget_file_path)
                free(set.widget_file_path);
            set.widget_file_path = (char*)malloc(strlen(val)+1);
            if (!set.widget_file_path)
                return 0;
            strcpy(set.widget_file_path, val);
            break;

        case SET_CONFIG_FILE_PATH:
            if (set.config_file_path)
                free(set.config_file_path);
            set.config_file_path = (char*)malloc(strlen(val)+1);
            if (!set.config_file_path)
                return 0;
            strcpy(set.config_file_path, val);
            break;

        default:
            return 0;
    }

    return 1;
}