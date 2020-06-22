/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_FAN_H_qwewqiorhq
#define MAC_FAN_CONTROL_FAN_H_qwewqiorhq

#include "config.h"

/**
 * Struct holding all information about one cooling fan.
 */
typedef struct fan {
    short int id;
    int min;
    int max;
    int speed;
    int step;
    char *write_path;
    struct fan *next;
} t_fan;

/**
 * Enum holding types of fan modes from applesmc.
 */
enum fan_mode {
    FAN_AUTO,
    FAN_MANUAL
};

/**
 * Loads max and min speed of each fan from appropriate files and counts step for each fan.
 * @param[in] fans Pointer to head of linked list of all fans.
 * @return 1 on success, 0 on error
 */
int load_fans_default_speed(t_settings *settings, t_fan *fans);

/**
 * Constructs linked list of all accessible fans in /sys/devices/platform/applesmc.768/.
 * @return Pointer to first fan in linked list, NULL on error
 */
t_fan *load_fans(t_settings *settings);

/**
 * Sets all fans to either auto or manual mode.
 * @param[in] fans Pointer to head of linked list of all fans.
 * @param[in] mode Fans mode (FAN_AUTO/FAN_MANUAL).
 * @return 1 on succes, 0 on error
 */
int set_fans_mode(t_fan *fans, enum fan_mode mode);

/**
 * Frees all fans in linked list.
 * @param[in] fans Pointer to head of linked list of all fans.
 */
void free_fans(t_fan *fans);

/**
 *
 * @param fan
 * @param speed
 */
int set_fan_speed(t_fan *fan, int speed);

#endif //MAC_FAN_CONTROL_FAN_H_qwewqiorhq
