/**
 * macfand - hipuranyhou - 22.01.2021
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
 * @brief Struct holding temperatures needed for adjusting fans.
 * Struct used in main control loop holding all temperatures (previous, real (current), delta of these two
 * and high, low and max from settings).
 */
struct ctrl_temps {
    int prev;
    int real;
    int dlt;
    int high;
    int low;
    int max;
};

/**
 * @brief Infinite loop adjusting fan speed based on current temperature.
 * Starts infinite loop which loads temperatures using control_set_temps(), calculates and sets new speed of every fan
 * in fans using control_calculate_speed() and fan_set_speed(). In case registered signal is catched, returns.
 * @param[in] mons Pointer to head of generic linked list of temperature monitors.
 * @param[in] fans Pointer to head of generic linked list of system fans.
 * @return int 0 on error, 1 on success
 */
int ctrl_start(const t_node *mons, t_node *fans);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf