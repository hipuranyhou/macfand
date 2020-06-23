/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_FAN_H_qwewqiorhq
#define MACFAND_FAN_H_qwewqiorhq

#include "config.h"
#include "linked.h"

/**
 * @brief 
 * 
 */
typedef struct fan {
    int id;
    int min;
    int max;
    int speed;
    int step;
    char *path_write;
    char *path_manual;
} t_fan;

/**
 * @brief 
 * 
 */
enum fan_mode {
    FAN_AUTO,
    FAN_MANUAL
};

/**
 * @brief 
 * 
 * @param settings 
 * @param fans 
 * @return int 
 */
int fan_load_defaults(const t_settings *settings, t_fan *fan);

/**
 * @brief 
 * 
 * @param fan 
 * @return int 
 */
int fan_id_exists(const int fan_cnt);

/**
 * @brief 
 * 
 * @param settings 
 * @return t_node* 
 */
t_node *fans_load(const t_settings *settings);

/**
 * @brief Set the fans mode object
 * 
 * @param fans 
 * @param mode 
 * @return int 
 */
int fans_set_mode(const t_node *fans, const enum fan_mode mode);

/**
 * @brief 
 * 
 * @param fan 
 */
void fan_free(t_fan *fan);

/**
 * @brief Set the fan speed object
 * 
 * @param fan 
 * @param speed 
 * @return int 
 */
int fan_set_speed(t_fan *fan, const int speed);

#endif //MACFAND_FAN_H_qwewqiorhq
