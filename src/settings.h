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
 * Enum holding all available settings, which are temperatures low, high and max. Poll time of fan adjust and
 * daemon and verbose modes.
 */
enum setting {
    SET_TEMP_LOW,
    SET_TEMP_HIGH,
    SET_TEMP_MAX,
    SET_TIME_POLL,
    SET_DAEMON,
    SET_VERBOSE,
    SET_LOG_TYPE,
    SET_LOG_FILE_PATH
};

/**
 * @brief Set the tings free object
 * 
 */
void settings_free();

/**
 * @brief Set the tings check object
 * 
 * @return int 
 */
int settings_check();

/**
 * @brief Gets setting value.
 * Gets value of given setting.
 * @param[in]  setting  Setting which value we want to get (one of enum setting).
 * @return int -1 on error, value of given settings otherwise.
 */
int settings_get_value(int setting);

/**
 * @brief Set the tings get value string object
 * 
 * @param setting 
 * @return char* 
 */
char* settings_get_value_string(int setting);

/**
 * @brief Sets setting value.
 * Sets value of given setting to value given in value.
 * @param[in]  setting  Setting which value we want to get (one of enum setting).
 * @param[in]  value    Value to be set (value >= 0)
 * @return int 0 on error, 1 on success.
 */
int settings_set_value(int setting, int value);

/**
 * @brief Set the tings set value object
 * 
 * @param setting 
 * @param value 
 * @return int 
 */
int settings_set_value_string(int setting, const char *value);

#endif //MACFAND_SETTINGS_H_jkdhfasjkf