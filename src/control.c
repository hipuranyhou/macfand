/**
 * macfand - hipuranyhou - 22.01.2021
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
static int ctrl_rld_conf(void);

/**
 * @brief Calculates fan target speed.
 * Calculates new fan speed based on temperatures stored in temps which will be fan->min if current temperature is 
 * under settings->temp_low, fan->max if current temperature is over settings->temp_max, or one of
 * (fan->min + fan->step * steps) and (fan->max - fan->step * steps) if fans need to cool more or less, respectively.
 * @param[in,out] temps Pointer to struct holding control temperature values.
 * @param[in]     fan   Pointer to current adjusted fan.
 */
static void ctrl_calc_spd(struct ctrl_temps *const temps, t_fan *const fan);

/**
 * @brief Adjusts temperatures in control.
 * Sets temp_previous to temp_current, updates temp_current using monitors_get_temp() and calculates temp_delta
 * based on these two updated values.
 * @param[in,out] temps Pointer to struct holding control temperature values.
 * @param[in]     mons  Pointer to head of generic linked list of temperature monitors.
 */
static void ctrl_set_temps(struct ctrl_temps *const temps, const t_node *mons);


volatile sig_atomic_t term_flag = 0;
volatile sig_atomic_t reload_flag = 0;


static int ctrl_rld_conf(void) {

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


static void ctrl_calc_speed(struct ctrl_temps *const temps, t_fan *const fan) {
    int steps = 0;

    fan->spd.tgt = fan->spd.real;

    // Extremes
    if (temps->real >= temps->max) {
        fan->spd.tgt = fan->spd.max;
        return;
    }

    if (temps->real <= temps->low) {
        fan->spd.tgt = fan->spd.min;
        return;
    }

    // Set higher or lower speed
    if (temps->dlt > 0 && temps->real > temps->high) {
        steps = (temps->real - temps->high) * (temps->real - temps->high + 1) / 2;
        fan->spd.tgt = max(fan->spd.tgt, (fan->spd.min + (steps * fan->spd.step)));
        return;
    }

    if (temps->dlt < 0 && temps->real > temps->low) {
        steps = (temps->low - temps->real) * (temps->low - temps->real + 1) / 2;
        fan->spd.tgt = min(fan->spd.tgt, (fan->spd.max - (steps * fan->spd.step)));
        return;
    }
}


static void ctrl_set_temps(struct ctrl_temps *const temps, const t_node *mons) {
    temps->prev = temps->real;
    temps->real = monitors_get_temp(mons);
    temps->dlt = temps->real - temps->prev;
}


int ctrl_start(const t_node *mons, t_node *fans) {
    struct ctrl_temps temps = {
        .prev = 0,
        .real = 0,
        .dlt = 0,
        .high = settings_get_value(SET_TEMP_HIGH),
        .low = settings_get_value(SET_TEMP_LOW),
        .max = settings_get_value(SET_TEMP_MAX),
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
            if (!ctrl_rld_conf()) {
                logger_log(LOG_L_ERROR, "Unable to reload configuration file");
                return 0;
            }
            logger_log(LOG_L_INFO, "Configuration file reloaded");
            reload_flag = 0;
        }

        // Prepare next fan loop
        fans = fans_head;
        ctrl_set_temps(&temps, mons);

        // Write widget file
        if (settings_get_value(SET_WIDGET))
            widget_write(fans);

        // Set speed of each fan
        while (fans) {
            fan = fans->data;
            ctrl_calc_spd(&temps, fan);
            if (!fan_write_spd(fan))
                logger_log(LOG_L_DEBUG, "Unable to set speed of fan %d", fan->id);
            fans = fans->next;
        }

        // Wait
        nanosleep(&ts, NULL);
    }

    return 1;
}