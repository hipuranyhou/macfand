/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_CONFIG_H_jkdhfasjkf
#define MAC_FAN_CONTROL_CONFIG_H_jkdhfasjkf

typedef struct settings {
    int low_temp;
    int high_temp;
    int max_temp;
    int poll_time;
    int daemon;
} t_settings;

void load_default_settings(t_settings *settings);

#endif //MAC_FAN_CONTROL_CONFIG_H_jkdhfasjkf