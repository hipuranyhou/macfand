/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "settings.h"
#include "logger.h"
#include "helper.h"

/**
 * @brief Assigns value of given setting.
 * Assigns value of given setting.
 * @param[in]  key    Name of setting to be assigned.
 * @param[in]  value  Value to be assigned.
 * @return int 0 on error, 1 on success.
 */
static int config_assign_setting(const char *key, const char *value);

/**
 * @brief Parses given line of configuration file.
 * Extracts key and value from given line of configuration file and assigns
 * value of setting with given key.
 * @param[in]  line  String holding line to be parsed.
 * @return int 0 on error, 1 on success.
 */
static int config_parse_line(const char *config_line);

/**
 * @brief Parses each line of given configuration file.
 * Parses each line of given configuration file.
 * @param[in]  config_file  Pointer to opened file to be parsed.
 * @return int -1 on error, 0 on success, line number with mistake otherwise.
 */
static int config_parse_file(FILE *config_file);


static int config_assign_setting(const char *key, const char *value) {
    int value_int = 0, is_boolean = 0;

    if (strcmp(value, "yes") == 0 || strcmp(value, "true") == 0) {
        is_boolean = 1;
        value_int = 1;
    }

    if (strcmp(value, "no") == 0 || strcmp(value, "false") == 0) {
        is_boolean = 1;
        value_int = 0;
    }

    // Char settings
    if (!get_int_from_string(value, &value_int) && !is_boolean) {

        // Log type
        if (strcmp(key, "log_type") == 0) {
            if (strcmp(value, "std") == 0) {
                if (!settings_set_value(SET_LOG_TYPE, LOG_T_STD))
                    return 0;
                return 1;
            }
            if (strcmp(value, "sys") == 0) {
                if (!settings_set_value(SET_LOG_TYPE, LOG_T_SYS))
                    return 0;
                return 1;
            }
            if (strcmp(value, "file") == 0) {
                if (!settings_set_value(SET_LOG_TYPE, LOG_T_FILE))
                    return 0;
                return 1;
            }
        }

        // Log file path
        if (strcmp(key, "log_file_path") == 0) {
            if (!settings_set_value_string(SET_LOG_FILE_PATH, value))
                return 0;
            return 1;
        }

        // Widget file path
        if (strcmp(key, "widget_file_path") == 0) {
            if (!settings_set_value_string(SET_WIDGET_FILE_PATH, value))
                return 0;
            return 1;
        }

        return 0;
    }
    
    // Integer settings
    if (strcmp(key, "temp_low") == 0) {
        if (!settings_set_value(SET_TEMP_LOW, value_int))
            return 0;
        return 1;
    }
    if (strcmp(key, "temp_high") == 0) {
        if (!settings_set_value(SET_TEMP_HIGH, value_int))
            return 0;
        return 1;
    }
    if (strcmp(key, "time_poll") == 0) {
        if (!settings_set_value(SET_TIME_POLL, value_int))
            return 0;
        return 1;
    }
    if (strcmp(key, "daemon") == 0) {
        if (!settings_set_value(SET_DAEMON, value_int))
            return 0;
        return 1;
    }
    if (strcmp(key, "verbose") == 0) {
        if (!settings_set_value(SET_VERBOSE, value_int))
            return 0;
        return 1;
    }
    if (strcmp(key, "widget") == 0) {
        if (!settings_set_value(SET_WIDGET, value_int))
            return 0;
        return 1;
    }

    return 0;
}


static int config_parse_line(const char *config_line) {
    char *key = NULL, *value = NULL;
    size_t key_size = 0, value_size = 0;
    size_t get_return = 0;
    int status = 0;

    if (!config_line)
        return 0;

    // Skip whitespace at the beginning of the line
    while (isspace(*config_line))
        config_line++;

    // Skip empty lines and comments
    if (strlen(config_line) == 0 || *config_line == '#')
        return 1;

    // Load key
    if ((get_return = get_line_until(config_line, ':', &key, &key_size)) < 1) {
        if (key)
            free(key);
        return 0;
    }

    // Skip whitespace before value
    config_line += get_return;
    while (isspace(*config_line) || *config_line == '"' || *config_line == ':')
        config_line++;

    // Load value
    if (get_line_until(config_line, '"', &value, &value_size) < 1) {
        free(key);
        if (value)
            free(value);
        return 0;
    }

    status = config_assign_setting(key, value);
    free(key);
    free(value);
    return status;
}


static int config_parse_file(FILE *config_file) {
    char *line = NULL;
    int cnt_line = 1;
    size_t buffer_size = 0;

    if (!config_file)
        return -1;

    errno = 0;

    while (getline(&line, &buffer_size, config_file) != -1) {
        if (!config_parse_line(line)) {
            if (line)
                free(line);
            return cnt_line;
        }
        cnt_line++;
    }

    if (line)
        free(line);

    if (errno != 0)
        return -1;

    return 0;
}


int config_load(const char *config_path) {
    FILE *config_file = NULL;
    int parse_return = 0;
    
    config_file = fopen((config_path) ? config_path : "/etc/macfand.conf", "r");
    if (!config_file)
        return 0;

    parse_return = config_parse_file(config_file);

    if (fclose(config_file) == EOF)
        logger_log(LOG_L_WARN, "%s", "Unable to close configuration file");

    if (parse_return == -1) {
        logger_log(LOG_L_DEBUG, "%s", "Unknown error encoutered while parsing configuration file");
        return 0;
    }
    if (parse_return > 0) {
        logger_log(LOG_L_DEBUG, "%s %d %s", "Unable to parse line", parse_return, "of configuraton file");
        return 0;
    }

    return 1;
}
