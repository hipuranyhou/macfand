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
typedef struct settings {
    int low_temp;
    int high_temp;
    int max_temp;
    int poll_time;
    int daemon;
    int verbose;
} t_settings;

/**
 * @brief 
 * 
 * @param settings 
 */
void settings_load_defaults(t_settings *settings);

/**
 * @brief 
 * 
 */
void settings_set_max_temp(t_settings *settings, const t_node *monitors);

#endif //MACFAND_SETTINGS_H_jkdhfasjkf