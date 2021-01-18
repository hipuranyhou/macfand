/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_CONTROL_H_fsdfdsfsdf
#define MACFAND_CONTROL_H_fsdfdsfsdf

#include "linked.h"

/**
 * @brief Holds values needed for adjusting fans.
 * Struct used in main control loop holding all three temperatures (previous, current and delta of these two),
 * number of fan speed adjust steps and current set speed (one of fan.min, fan.max, fan.min + fan.step * steps)
 * which are all used for adjusting fan speed.
 */
typedef struct control {
    int temp_previous;
    int temp_current;
    int temp_delta;
    int speed;
    int steps;
} t_control;

/**
 * @brief Infinite loop adjusting fan speed based on current temperature.
 * Starts infinite loop which loads temperatures using control_set_temps(), calculates and sets new speed of every fan
 * in fans using control_calculate_speed() and fan_set_speed(). In case registered signal is catched, returns.
 * @param[in,out]  fans      Pointer to head of linked list of system fans.
 * @param[in]      monitors  Pointer to head of linked list of temperature monitors.
 * @return int 0 on error, 1 on success
 */
int control_start(const t_node *monitors, t_node *fans);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf