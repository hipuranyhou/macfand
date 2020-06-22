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
    short int id;
    int min;
    int max;
    int speed;
    int step;
    char *write_path;
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
int load_fans_defaults(const t_settings *settings, t_node *fans);

/**
 * @brief 
 * 
 * @param settings 
 * @return t_node* 
 */
t_node *load_fans(const t_settings *settings);

/**
 * @brief Set the fans mode object
 * 
 * @param fans 
 * @param mode 
 * @return int 
 */
int set_fans_mode(t_node *fans, const enum fan_mode mode);

/**
 * @brief 
 * 
 * @param fan 
 */
void free_fan(t_fan *fan);

/**
 * @brief Set the fan speed object
 * 
 * @param fan 
 * @param speed 
 * @return int 
 */
int set_fan_speed(t_fan *fan, const int speed);

#endif //MACFAND_FAN_H_qwewqiorhq
