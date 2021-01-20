/**
 * macfand - hipuranyhou - 20.01.2021
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
typedef struct ctrl {
    int temp_prev;
    int temp_now;
    int temp_dlt;
    int speed;
    int steps;
} t_ctrl;

/**
 * @brief Infinite loop adjusting fan speed based on current temperature.
 * Starts infinite loop which loads temperatures using control_set_temps(), calculates and sets new speed of every fan
 * in fans using control_calculate_speed() and fan_set_speed(). In case registered signal is catched, returns.
 * @param[in] fans Pointer to head of linked list of system fans.
 * @param[in] mons Pointer to head of linked list of temperature monitors.
 * @return int 0 on error, 1 on success
 */
int ctrl_start(const t_node *mons, t_node *fans);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf