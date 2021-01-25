/**
 * macfand - hipuranyhou - 25.01.2021
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
 * @brief Assigns value of given integer setting.
 * Assigns value of given integer setting.
 * @param[in] key Name of setting to be assigned.
 * @param[in] val Integer to be assigned.
 * @return int 0 on error, 1 on success.
 */
static int conf_assign_int(const char *key, const int val);

/**
 * @brief Assigns value of given string setting.
 * Assigns value of given string setting.
 * @param[in] key Name of setting to be assigned.
 * @param[in] val String to be assigned.
 * @return int 0 on error, 1 on success.
 */
static int conf_assign_str(const char *key, const char *val);

/**
 * @brief Assigns value of given setting.
 * Assigns value of given setting.
 * @param[in] key Name of setting to be assigned.
 * @param[in] val Value to be assigned.
 * @return int 0 on error, 1 on success.
 */
static int conf_assign_set(const char *key, const char *val);

/**
 * @brief Parses given line of configuration file.
 * Extracts key and value from given line of configuration file and assigns
 * value of setting with given key.
 * @param[in] line String holding line to be parsed.
 * @return int 0 on error, 1 on success.
 */
static int conf_parse_line(const char *line);

/**
 * @brief Parses each line of given configuration file.
 * Parses each line of given configuration file.
 * @param[in] fp Pointer to opened config file to be parsed.
 * @return int -1 on error, 0 on success, line number with mistake otherwise.
 */
static int conf_parse_file(FILE *fp);


static int conf_assign_int(const char *key, const int val) {
    
    if (strcmp(key, "temp_low") == 0) {
        if (!set_set_val(SET_TEMP_LOW, val))
            return 0;
    } else if (strcmp(key, "temp_high") == 0) {
        if (!set_set_val(SET_TEMP_HIGH, val))
            return 0;
    } else if (strcmp(key, "time_poll") == 0) {
        if (!set_set_val(SET_TIME_POLL, val))
            return 0;
    } else if (strcmp(key, "daemon") == 0) {
        if (!set_set_val(SET_DAEMON, val))
            return 0;
    } else if (strcmp(key, "verbose") == 0) {
        if (!set_set_val(SET_VERBOSE, val))
            return 0;
    } else if (strcmp(key, "widget") == 0) {
        if (!set_set_val(SET_WIDGET, val))
            return 0;
    } else
        return 0;

    return 1;
}


static int conf_assign_str(const char *key, const char *val) {

    // Log type
    if (strcmp(key, "log_type") == 0) {

        if (strcmp(val, "std") == 0) {
            if (!set_set_val(SET_LOG_TYPE, LOG_T_STD))
                return 0;
        } else if (strcmp(val, "sys") == 0) {
            if (!set_set_val(SET_LOG_TYPE, LOG_T_SYS))
                return 0;
        } else if (strcmp(val, "file") == 0) {
            if (!set_set_val(SET_LOG_TYPE, LOG_T_FILE))
                return 0;
        } else
            return 0;

    } else if (strcmp(key, "log_file_path") == 0) {
        if (!set_set_val_str(SET_LOG_FILE_PATH, val))
            return 0;
    } else if (strcmp(key, "widget_file_path") == 0) {
        if (!set_set_val_str(SET_WIDGET_FILE_PATH, val))
            return 0;
    } else
        return 0;

    return 1;
}


static int conf_assign_set(const char *key, const char *val) {
    int val_int = 0;
    int is_bool = 0;

    // Check toogle value
    if (strcmp(val, "yes") == 0 || strcmp(val, "true") == 0) {
        is_bool = 1;
        val_int = 1;
    } else if (strcmp(val, "no") == 0 || strcmp(val, "false") == 0) {
        is_bool = 1;
        val_int = 0;
    }

    // String settings
    if (str_to_int(val, &val_int, 10, NULL) > 0 && !is_bool)
        return conf_assign_str(key, val);
    
    // Integer settings
    return conf_assign_int(key, val_int);
}


static int conf_parse_line(const char *line) {
    char    *key     = NULL;
    char    *val     = NULL;
    size_t  key_size = 0;
    size_t  val_size = 0;
    ssize_t get_ret  = 0;
    int     ret      = 0;

    if (!line)
        return 0;

    // Skip whitespace at the beginning of the line
    while (isspace(*line))
        line++;

    // Skip empty lines and comments
    if (strlen(line) == 0 || *line == '#')
        return 1;

    // Load key
    get_ret = get_str_until(line, ':', &key, &key_size);
    if (get_ret < 1) {
        if (key)
            free(key);
        return 0;
    }

    // Skip whitespace before value (+ 1 for ':')
    line += get_ret + 1;
    while (isspace(*line) || *line == '"')
        line++;

    // Load value
    get_ret = get_str_until(line, '"', &val, &val_size);
    if (get_ret < 1) {
        free(key);
        if (val)
            free(val);
        return 0;
    }

    ret = conf_assign_set(key, val);

    free(key);
    free(val);
    return ret;
}


static int conf_parse_file(FILE *fp) {
    char   *line     = NULL;
    int    line_cnt  = 1;
    int    parse_ret = 0;
    size_t buff_size = 0;

    if (!fp)
        return -1;

    errno = 0;
    while (getline(&line, &buff_size, fp) > -1) {
        parse_ret = conf_parse_line(line);

        if (parse_ret == 0)
            break;
        
        line_cnt++;
    }

    if (line)
        free(line);

    if (errno != 0)
        return -1;
    else if (parse_ret == 0)
        return line_cnt;
    else
        return 0;
}


int conf_load(const char *path) {
    FILE *fp       = NULL;
    int  parse_ret = 0;
    
    fp = fopen((path) ? path : "/etc/macfand.conf", "r");
    if (!fp)
        return 0;

    parse_ret = conf_parse_file(fp);

    if (fclose(fp) == EOF)
        log_log(LOG_L_WARN, "Unable to close configuration file");

    if (parse_ret < 0) {
        log_log(LOG_L_DEBUG, "Unknown error encoutered while parsing configuration file");
        return 0;
    } else if (parse_ret > 0) {
        log_log(LOG_L_DEBUG, "Unable to parse line %d of configuraton file", parse_ret);
        return 0;
    }

    return 1;
}
