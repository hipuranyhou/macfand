/**
 * macfand - hipuranyhou - 20.01.2021
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
static int ctrl_reload_conf(void);

/**
 * @brief Calculates new fan speed.
 * Calculates new fan speed based on temperatures stored in control which will be fan->min if current temperature is 
 * under settings->temp_low, fan->max if current temperature is over settings->temp_max, or one of
 * (fan->min + fan->step * steps) and (fan->max - fan->step * steps) if fans need to cool more or less, respectively.
 * @param[in,out] ctrl Pointer to struct holding control values.
 * @param[in]     fan  Pointer to current adjusted fan.
 */
static void ctrl_calc_speed(t_ctrl *ctrl, const t_fan *fan);

/**
 * @brief Adjusts temperatures in control.
 * Sets temp_previous to temp_current, updates temp_current using monitors_get_temp() and calculates temp_delta
 * based on these two updated values.
 * @param[in,out] ctrl Pointer to struct holding control values.
 * @param[in]     mons Pointer to head of linked list of temperature monitors.
 */
static void ctrl_set_temps(t_ctrl *ctrl, const t_node *mons);


volatile sig_atomic_t term_flag = 0;
volatile sig_atomic_t reload_flag = 0;


static int ctrl_reload_conf(void) {

    if (!conf_load(settings_get_value_string(SET_CONFIG_FILE_PATH))) {
        logger_log(LOG_L_ERROR, "Unable to load configuration file");
        return 0;
    }

    if (!settings_check()) {
        logger_log(LOG_L_ERROR, "Settings are invalid");
        return 0;
    }

    if (!logger_set_type(settings_get_value(SET_LOG_TYPE), settings_get_value_string(SET_LOG_FILE_PATH))) {
        logger_log(LOG_L_ERROR, "Unable to set logger to configured mode");
        return 0;
    }

    return 1;
}


static void ctrl_calc_speed(t_ctrl *ctrl, const t_fan *fan) {
    int set_temp_high = 0;
    int set_temp_low  = settings_get_value(SET_TEMP_LOW);

    ctrl->speed = fan->speed;

    // Extremes
    if (ctrl->temp_now >= settings_get_value(SET_TEMP_MAX)) {
        ctrl->speed = fan->max;
        return;
    }

    if (ctrl->temp_now <= set_temp_low) {
        ctrl->speed = fan->min;
        return;
    }

    // Set higher or lower speed
    set_temp_high = settings_get_value(SET_TEMP_HIGH);
    if (ctrl->temp_dlt > 0 && ctrl->temp_now > set_temp_high) {
        ctrl->steps = (ctrl->temp_now - set_temp_high) * (ctrl->temp_now - set_temp_high + 1) / 2;
        ctrl->speed = max(ctrl->speed, (fan->min + (ctrl->steps * fan->step)));
        return;
    }

    if (ctrl->temp_dlt < 0 && ctrl->temp_now > set_temp_low) {
        ctrl->steps = (set_temp_low - ctrl->temp_now) * (set_temp_low - ctrl->temp_now + 1) / 2;
        ctrl->speed = min(ctrl->speed, (fan->max - (ctrl->steps * fan->step)));
        return;
    }
}


static void ctrl_set_temps(t_ctrl *ctrl, const t_node *mons) {
    ctrl->temp_prev = ctrl->temp_now;
    ctrl->temp_now = monitors_get_temp(mons);
    ctrl->temp_dlt = ctrl->temp_now - ctrl->temp_prev;
}


int ctrl_start(const t_node *mons, t_node *fans) {
    t_ctrl ctrl = {
        .temp_prev = 0, 
        .temp_now = 0, 
        .temp_dlt = 0, 
        .speed = 0, 
        .steps = 0
    };
    struct timespec ts = {
        .tv_sec = settings_get_value(SET_TIME_POLL),
        .tv_nsec = 0
    };
    t_fan  *fan       = NULL;
    t_node *fans_head = fans;
    

    if (!fans || !mons)
        return 0;

    for(;;) {
        if (term_flag)
            return 1;

        // SIGHUP catched for reloading of config
        if (reload_flag) {
            if (!ctrl_reload_conf()) {
                logger_log(LOG_L_ERROR, "Unable to reload configuration file");
                return 0;
            }
            logger_log(LOG_L_INFO, "Configuration file reloaded");
            reload_flag = 0;
        }

        // Prepare next fan loop
        fans = fans_head;
        ctrl_set_temps(&ctrl, mons);

        // Write widget file
        if (settings_get_value(SET_WIDGET))
            widget_write(fans);

        // Set speed of each fan
        while (fans) {
            fan = fans->data;
            ctrl_calc_speed(&ctrl, fan);
            if (!fan_set_speed(fan, ctrl.speed))
                logger_log(LOG_L_DEBUG, "Unable to set speed of fan %d", fan->id);
            fans = fans->next;
        }

        // Wait
        nanosleep(&ts, NULL);
    }

    return 1;
}