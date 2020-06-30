/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_CONTROL_H_fsdfdsfsdf
#define MACFAND_CONTROL_H_fsdfdsfsdf

#include "settings.h"
#include "fan.h"
#include "monitor.h"
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
 * @brief Calculates new fan speed.
 * Calculates new fan speed based on temperatures stored in control which will be fan->min if current temperature is 
 * under settings->temp_low, fan->max if current temperature is over settings->temp_max, or one of
 * (fan->min + fan->step * steps) and (fan->max - fan->step * steps) if fans need to cool more or less, respectively.
 * @param[in,out]  control   Pointer to struct holding control values.
 * @param[in]      fan       Pointer to current adjusted fan.
 */
static void control_calculate_speed(t_control *control, const t_fan *fan);

/**
 * @brief Adjusts temperatures in control.
 * Sets temp_previous to temp_current, updates temp_current using monitors_get_temp() and calculates temp_delta
 * based on these two updated values.
 * @param[in,out]  control   Pointer to struct holding control values.
 * @param[in]      monitors  Pointer to head of linked list of temperature monitors.
 */
static void control_set_temps(t_control *control, const t_node *monitors);

/**
 * @brief Infinite loop adjusting fan speed based on current temperature.
 * Starts infinite loop which loads temperatures using control_set_temps(), calculates and sets new speed of every fan
 * in fans using control_calculate_speed() and fan_set_speed(). Does this every settings->time_poll. In case
 * registered signal is catched, returns.
 * @param[in,out]  fans      Pointer to head of linked list of system fans.
 * @param[in]      monitors  Pointer to head of linked list of temperature monitors.
 */
void control_start(t_node *fans, const t_node *monitors);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf