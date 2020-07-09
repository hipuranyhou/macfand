/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_SETTINGS_H_jkdhfasjkf
#define MACFAND_SETTINGS_H_jkdhfasjkf

#include "linked.h"

/**
 * @brief Enum holding all available settings.
 * Enum holding all available settings, which are temperatures low, high and max. 
 * Poll time of fan adjust and daemon and verbose modes.
 */
enum setting {
    SET_TEMP_LOW,
    SET_TEMP_HIGH,
    SET_TEMP_MAX,
    SET_TIME_POLL,
    SET_DAEMON,
    SET_VERBOSE,
    SET_LOG_TYPE,
    SET_LOG_FILE_PATH,
    SET_WIDGET,
    SET_WIDGET_FILE_PATH,
    SET_CONFIG_FILE_PATH
};

/**
 * @brief Frees memory used by settings.
 * Frees memory used by settings.
 */
void settings_free();

/**
 * @brief Checks validity of settings.
 * Checks validity of settings. For string settings sets default values if they 
 * are NULL (log file, i3 widget, ...).
 * @return int 0 on error, 1 on success
 */
int settings_check();

/**
 * @brief Gets setting integer value.
 * Gets integer value of given setting.
 * @param[in]  setting  Setting which value we want to get (one of enum setting).
 * @return int -1 on error, value of given settings otherwise.
 */
int settings_get_value(const int setting);

/**
 * @brief Gets setting string value.
 * Gets string value of given setting.
 * @param[in]  setting  Setting which value we want to get (one of enum setting).
 * @return char* NULL on error, string otherwise
 */
char* settings_get_value_string(const int setting);

/**
 * @brief Sets setting integer value.
 * Sets integer value of given setting to value. Does not check validity of given setting.
 * @param[in]  setting  Setting which value we want to set (one of enum setting).
 * @param[in]  value    Value to be set.
 * @return int 0 on error, 1 on success.
 */
int settings_set_value(const int setting, const int value);

/**
 * @brief Set the tings set value object
 * Sets string value of given setting to value. Allocates memory which has to be freed
 * by caller using settings_free(). Does not check validity of given setting.
 * @param[in]  setting  Setting which value we want to set (one of enum setting).
 * @param[in]  value    Value to be set.
 * @return int 0 on error, 1 on success.
 */
int settings_set_value_string(const int setting, const char *value);

#endif //MACFAND_SETTINGS_H_jkdhfasjkf