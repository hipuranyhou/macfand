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


volatile int termination_flag = 0;


void control_calculate_speed(const t_settings *settings, t_control *control, const t_fan *fan) {
    control->speed = fan->speed;

    // Extremes
    if (control->temp_current >= settings->temp_max) {
        control->speed = fan->max;
        return;
    }

    if (control->temp_current <= settings->temp_low) {
        control->speed = fan->min;
        return;
    }

    // Set lower or higher
    if (control->temp_delta > 0 && control->temp_current > settings->temp_high) {
        control->steps = (control->temp_current - settings->temp_high) * (control->temp_current - settings->temp_high + 1) / 2;
        control->speed = max(control->speed, (fan->min + (control->steps * fan->step)));
        return;
    }

    if (control->temp_delta < 0 && control->temp_current > settings->temp_low) {
        control->steps = (settings->temp_max - control->temp_current) * (settings->temp_max - control->temp_current + 1) / 2;
        control->speed = min(control->speed, (fan->max - (control->steps * fan->step)));
        return;
    }
}


void control_set_temps(t_control *control, const t_node *monitors) {
    control->temp_previous = control->temp_current;
    control->temp_current = monitors_get_temp(monitors);
    control->temp_delta = control->temp_current - control->temp_previous;
}


void control_start(const t_settings *settings, t_node *fans, const t_node *monitors) {
    t_control control = {0, 0, 0, 0, 0};
    t_fan *fan = NULL;
    t_node *fans_head = fans;
    struct timespec ts;

    if (!settings || !fans || !monitors)
        return;

    ts.tv_sec = settings->time_poll;
    ts.tv_nsec = 0;

    for(;;) {
        // TODO: Remove pid? and log
        if (termination_flag)
            return;

        // Prepare next fan loop
        fans = fans_head;
        control_set_temps(&control, monitors);

        // Set speed of each fan
        while (fans) {
            fan = fans->data;
            control_calculate_speed(settings, &control, fan);
            // TODO: log error
            fan_set_speed(fan, control.speed);
            //printf("Fan %d -> %d (%d)\n", fan->id, fan->speed, control.temp_current);
            fans = fans->next;
        }
        // Wait
        nanosleep(&ts, NULL);
    }
}