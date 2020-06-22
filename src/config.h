/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_CONFIG_H_jkdhfasjkf
#define MACFAND_CONFIG_H_jkdhfasjkf

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
} t_settings;


/**
 * @brief 
 * 
 * @param settings 
 */
void load_default_settings(t_settings *settings);

#endif //MACFAND_CONFIG_H_jkdhfasjkf