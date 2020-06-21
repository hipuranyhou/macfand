//
// Created by hipuranyhou on 21.06.20.
//

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