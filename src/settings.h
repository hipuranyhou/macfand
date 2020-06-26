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
 * @brief Holds macfand settings values.
 * Used to hold macfand settings, all temperatures used for calculating fan adjust (low, high, max), 
 * main control loop poll time and if macfand should run in daemon and verbose mode.
 */
typedef struct settings {
    int temp_low;
    int temp_high;
    int temp_max;
    int time_poll;
    int daemon;
    int verbose;
} t_settings;

/**
 * @brief Sets default macfand settings.
 * Sets default macfand settings.
 * @param[out]  settings  Pointer to struct holding macfand settings.
 */
void settings_load_defaults(t_settings *settings);

/**
 * @brief Sets settings->temp_max.
 * Sets settings->temp_max based on highest value got from temperature monitors (monitor->temp_max).
 * @param[out]  settings  Pointer to struct holding macfand settings.
 * @param[in]   monitors  Pointer to head of linked list of temperature monitors.
 */
void settings_set_max_temp(t_settings *settings, const t_node *monitors);

#endif //MACFAND_SETTINGS_H_jkdhfasjkf