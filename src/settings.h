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
 * @brief 
 * 
 */
enum setting {
    SET_TEMP_LOW,
    SET_TEMP_HIGH,
    SET_TEMP_MAX,
    SET_TIME_POLL,
    SET_DAEMON,
    SET_VERBOSE
};

/**
 * @brief
 * 
 * @param setting 
 * @return int 
 */
int settings_get_value(int setting);

/**
 * @brief
 * 
 * @param setting 
 * @param value 
 * @return int 
 */
int settings_set_value(int setting, int value);

#endif //MACFAND_SETTINGS_H_jkdhfasjkf