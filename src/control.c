/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <time.h>
#include <syslog.h>
#include <signal.h>

#include "control.h"
#include "helper.h"
#include "config.h"
#include "logger.h"
#include "settings.h"
#include "fan.h"
#include "monitor.h"
#include "widget.h"
#include "daemonize.h"

/**
 * @brief Reloads settings from configuration file.
 * Reloads settings from configuration file, check its validity and reloads logger.
 * @return int 0 on error, 1 on success.
 */
static int control_reload_config(void);

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


volatile sig_atomic_t termination_flag = 0;
volatile sig_atomic_t reload_config_flag = 0;


static int control_reload_config(void) {

    if (!config_load(settings_get_value_string(SET_CONFIG_FILE_PATH))) {
        logger_log(LOG_L_ERROR, "%s", "Unable to load configuration file");
        return 0;
    }

    if (!settings_check()) {
        logger_log(LOG_L_ERROR, "%s", "Settings are invalid");
        return 0;
    }

    if (!logger_set_type(settings_get_value(SET_LOG_TYPE), settings_get_value_string(SET_LOG_FILE_PATH))) {
        logger_log(LOG_L_ERROR, "%s", "Unable to set logger to configured mode");
        return 0;
    }

    return 1;
}


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


int control_start(const t_node *monitors, t_node *fans) {
    t_control control = {
        .temp_previous = 0, 
        .temp_current = 0, 
        .temp_delta = 0, 
        .speed = 0, 
        .steps = 0
    };
    t_fan *fan = NULL;
    t_node *fans_head = fans;
    struct timespec ts;

    if (!fans || !monitors)
        return 0;

    ts.tv_sec = settings_get_value(SET_TIME_POLL);
    ts.tv_nsec = 0;

    for(;;) {
        if (termination_flag)
            return 1;
        if (reload_config_flag) {
            if (!control_reload_config()) {
                logger_log(LOG_L_ERROR, "%s", "Unable to reload configuration file");
                return 0;
            }
            logger_log(LOG_L_INFO, "%s", "Configuration file reloaded");
            reload_config_flag = 0;
        }

        // Prepare next fan loop
        fans = fans_head;
        control_set_temps(&control, monitors);

        // Write widget file
        if (settings_get_value(SET_WIDGET))
            widget_write(fans);

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

    return 1;
}