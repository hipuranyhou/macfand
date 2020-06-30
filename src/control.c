/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <time.h>
#include <syslog.h>

#include "control.h"
#include "helper.h"
#include "logger.h"
#include "settings.h"
#include "fan.h"
#include "monitor.h"

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


volatile int termination_flag = 0;


static void control_calculate_speed(t_control *control, const t_fan *fan) {
    control->speed = fan->speed;

    // Extremes
    if (control->temp_current >= settings_get_value(SET_TEMP_MAX)) {
        control->speed = fan->max;
        return;
    }

    if (control->temp_current <= settings_get_value(SET_TEMP_LOW)) {
        control->speed = fan->min;
        return;
    }

    // Set higher or lower speed
    if (control->temp_delta > 0 && control->temp_current > settings_get_value(SET_TEMP_HIGH)) {
        control->steps = (control->temp_current - settings_get_value(SET_TEMP_HIGH)) * (control->temp_current - settings_get_value(SET_TEMP_HIGH) + 1) / 2;
        control->speed = max(control->speed, (fan->min + (control->steps * fan->step)));
        return;
    }

    if (control->temp_delta < 0 && control->temp_current > settings_get_value(SET_TEMP_LOW)) {
        control->steps = (settings_get_value(SET_TEMP_LOW) - control->temp_current) * (settings_get_value(SET_TEMP_LOW) - control->temp_current + 1) / 2;
        control->speed = min(control->speed, (fan->max - (control->steps * fan->step)));
        return;
    }
}


static void control_set_temps(t_control *control, const t_node *monitors) {
    control->temp_previous = control->temp_current;
    control->temp_current = monitors_get_temp(monitors);
    control->temp_delta = control->temp_current - control->temp_previous;
}


void control_start(t_node *fans, const t_node *monitors) {
    t_control control = {0, 0, 0, 0, 0};
    t_fan *fan = NULL;
    t_node *fans_head = fans;
    struct timespec ts;

    if (!fans || !monitors)
        return;

    ts.tv_sec = settings_get_value(SET_TIME_POLL);
    ts.tv_nsec = 0;

    for(;;) {
        if (termination_flag)
            return;

        // Prepare next fan loop
        fans = fans_head;
        control_set_temps(&control, monitors);

        // Set speed of each fan
        while (fans) {
            fan = fans->data;
            control_calculate_speed(&control, fan);
            if (!fan_set_speed(fan, control.speed))
                logger_log(LOG_L_DEBUG, "%s %d", "Unable to set speed of fan", fan->id);
            fans = fans->next;
        }
        // Wait
        nanosleep(&ts, NULL);
    }
}