//
// Created by hipuranyhou on 21.06.20.
//

#include <stdio.h>
#include <time.h>
#include <syslog.h>

#include "control.h"

volatile int termination_flag = 0;

void start_control(t_settings *settings, t_fan *fans, t_monitor *monitors) {
    int prev_temp, cur_temp = 0, speed, steps, delta_temp;
    t_fan *fan = NULL;
    struct timespec ts;
    ts.tv_sec = settings->poll_time;
    ts.tv_nsec = 0;

    for(;;) {

        if (termination_flag) {
            if (!set_fans_mode(fans, FAN_AUTO)) {
                switch (settings->daemon) {
                    case 0:
                        fprintf(stderr, "Error while resetting fans to automatic mode.");
                        break;
                    case 1:
                        syslog(LOG_ERR, "Error while resetting fans to automatic mode.");
                        break;
                }
            }
            return;
        }

        // Prepare loop
        fan = fans;
        prev_temp = cur_temp;
        cur_temp = get_current_temp(monitors);
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
            if (!set_fan_speed(fan, speed)) {
                switch (settings->daemon) {
                    case 0:
                        fprintf(stderr, "%s%d\n", "Error while settings speed of fan ", fan->id);
                        break;
                    case 1:
                        syslog(LOG_ERR, "%s%d", "Error while settings speed of fan ", fan->id);
                        break;
                }
            }
            fan = fan->next;
        }

        nanosleep(&ts, NULL);
    }
}