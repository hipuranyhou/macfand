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

typedef struct t_control {
    int temp_previous;
    int temp_current;
    int temp_delta;
    int speed;
    int steps;
} t_control;

/**
 * @brief 
 * 
 * @param settings 
 * @param control 
 * @param fan 
 */
void control_calculate_speed(const t_settings *settings, t_control *control, const t_fan *fan);

/**
 * @brief 
 * 
 * @param control 
 * @param monitors 
 */
void control_set_temps(t_control *control, const t_node *monitors);

/**
 * @brief 
 * 
 * @param settings 
 * @param fans 
 * @param monitors 
 */
void control_start(const t_settings *settings, t_node *fans, const t_node *monitors);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf