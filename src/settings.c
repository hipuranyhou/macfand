/**
 * macfand - hipuranyhou - 21.06.2020
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
} settings = {
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


void settings_free() {
    if (settings.log_file_path)
        free(settings.log_file_path);
    if (settings.widget_file_path)
        free(settings.widget_file_path);
    if (settings.config_file_path)
        free(settings.config_file_path);
}


int settings_check() {

    if (settings.temp_low < 1) {
        logger_log(LOG_L_DEBUG, "%s", "Value of temp_low must be >= 1");
        return 0;
    }

    if (settings.temp_high <= settings.temp_low) {
        logger_log(LOG_L_DEBUG, "%s", "Value of temp_high is invalid (must be > temp_low)");
        return 0;
    }

    if (settings.temp_max <= settings.temp_high) {
        logger_log(LOG_L_DEBUG, "%s", "Value of temp_max is invalid (must be > temp_high");
        return 0;
    }

    if (settings.time_poll < 1) {
        logger_log(LOG_L_DEBUG, "%s", "Value of time_poll must be >= 1");
        return 0;
    }

    if (settings.daemon != 0 && settings.daemon != 1) {
        logger_log(LOG_L_DEBUG, "%s", "Value of daemon must be 0 or 1");
        return 0;
    }

    if (settings.verbose != 0 && settings.verbose != 1) {
        logger_log(LOG_L_DEBUG, "%s", "Value of verbose must be 0 or 1");
        return 0;
    }

    if (settings.log_type < LOG_T_STD || settings.log_type > LOG_T_FILE) {
        logger_log(LOG_L_DEBUG, "%s", "Value of log_type must be one of std, sys and file");
        return 0;
    }

    if (settings.log_type == LOG_T_FILE && !settings.log_file_path) {
        if (!settings_set_value_string(SET_LOG_FILE_PATH, "/var/log/macfand.log")) {
            logger_log(LOG_L_DEBUG, "%s", "Unable to set default log file path to /var/log/macfand.log");
            return 0;
        }
        logger_log(LOG_L_INFO, "%s", "Using default log file path /var/log/macfand.log");
    }

    if (settings.widget != 0 && settings.widget != 1) {
        logger_log(LOG_L_DEBUG, "%s", "Value of widget must be 0 or 1");
        return 0;
    }

    if (settings.widget && !settings.widget_file_path) {
        if (!settings_set_value_string(SET_WIDGET_FILE_PATH, "/tmp/macfand.widget")) {
            logger_log(LOG_L_DEBUG, "%s", "Unable to set default widget file path to /tmp/macfand.widget");
            return 0;
        }
        logger_log(LOG_L_INFO, "%s", "Using default widget file path /tmp/macfand.widget");
    }

    return 1;
}


int settings_get_value(const int setting) {
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
        case SET_LOG_TYPE:
            return settings.log_type;
        case SET_WIDGET:
            return settings.widget;
        default:
            return -1;
    }
}


char* settings_get_value_string(const int setting) {
    switch (setting) {
        case SET_LOG_FILE_PATH:
            return settings.log_file_path;
        case SET_WIDGET_FILE_PATH:
            return settings.widget_file_path;
        case SET_CONFIG_FILE_PATH:
            return settings.config_file_path;
        default:
            return NULL;
    }
}


int settings_set_value(const int setting, const int value) {
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
        case SET_LOG_TYPE:
            settings.log_type = value;
            break;
        case SET_WIDGET:
            settings.widget = value;
            break;
        default:
            return 0;
    }

    return 1;
}


int settings_set_value_string(const int setting, const char *value) { 
    if (!value)
        return 0;

    switch (setting) {

        case SET_LOG_FILE_PATH:
            if (settings.log_file_path)
                free(settings.log_file_path);
            settings.log_file_path = (char*)malloc(strlen(value)+1);
            if (!settings.log_file_path)
                return 0;
            strcpy(settings.log_file_path, value);
            break;

        case SET_WIDGET_FILE_PATH:
            if (settings.widget_file_path)
                free(settings.widget_file_path);
            settings.widget_file_path = (char*)malloc(strlen(value)+1);
            if (!settings.widget_file_path)
                return 0;
            strcpy(settings.widget_file_path, value);
            break;

        case SET_CONFIG_FILE_PATH:
            if (settings.config_file_path)
                free(settings.config_file_path);
            settings.config_file_path = (char*)malloc(strlen(value)+1);
            if (!settings.config_file_path)
                return 0;
            strcpy(settings.config_file_path, value);
            break;

        default:
            return 0;
    }

    return 1;
}