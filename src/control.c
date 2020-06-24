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

volatile int termination_flag = 0;

void start_control(const t_settings *settings, t_node **fans, const t_node *monitors) {
    int prev_temp = 0, cur_temp = 0, speed = 0, steps = 0, delta_temp = 0;
    t_fan *fan = NULL;
    struct timespec ts;
    ts.tv_sec = settings->poll_time;
    ts.tv_nsec = 0;

    for(;;) {

        // Exit after catched signal
        if (termination_flag)
            return;

        // Prepare loop
        *fans = fans;
        fan = (*fans)->data;
        prev_temp = cur_temp;
        cur_temp = monitors_get_temp(monitors);
        delta_temp = cur_temp - prev_temp;

        // Set speed of each fan
        while (fan) {
            speed = fan->speed;

            // Extremes
            if (cur_temp >= settings->max_temp)
                speed = fan->max;
            if (cur_temp <= settings->low_temp)
                speed = fan->min;

            // Set lower or higher
            if (delta_temp > 0 && cur_temp > settings->high_temp && cur_temp < settings->max_temp) {
                steps = (cur_temp - settings->high_temp) * (cur_temp - settings->high_temp + 1) / 2;
                speed = max(speed, (fan->min + (steps * fan->step)));
            }
            if (delta_temp < 0 && cur_temp > settings->low_temp && cur_temp < settings->max_temp) {
                steps = (settings->max_temp - cur_temp) * (settings->max_temp - cur_temp + 1) / 2;
                speed = min(speed, (fan->max - (steps * fan->step)));
            }

            // Set fan and move to next
            if (!fan_set_speed(fan, speed)) {
                switch (settings->daemon) {
                    case 0:
                        fprintf(stderr, "%s%d\n", "Error while settings speed of fan ", fan->id);
                        break;
                    case 1:
                        syslog(LOG_ERR, "%s%d", "Error while settings speed of fan ", fan->id);
                        break;
                }
            }
            *fans = (*fans)->next;
            fan = (*fans)->data;

            printf("Fan %d -> %d (%d)\n", fan->id, fan->speed, cur_temp);
        }

        nanosleep(&ts, NULL);
    }
}