/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */
#include <stdio.h>
#include <stdlib.h>

#include "fan.h"
#include "helper.h"

#define FAN_PATH_BASE "/sys/devices/platform/applesmc.768/fan"
#define FAN_PATH_WRITE "_output"
#define FAN_PATH_MAX "_max"
#define FAN_PATH_MIN "_min"
#define FAN_PATH_MANUAL "_manual"

int load_fans_default_speed(t_settings *settings, t_fan* fans) {
    char *max_path = NULL, *min_path = NULL;
    FILE *max_ptr = NULL, *min_ptr = NULL;

    while (fans) {
        max_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fans->id, FAN_PATH_MAX);
        min_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fans->id, FAN_PATH_MIN);
        max_ptr = fopen(max_path, "r");
        min_ptr = fopen(min_path, "r");

        if (max_ptr == NULL || min_ptr == NULL) {
            if (max_ptr != NULL)
                fclose(max_ptr);
            if (min_ptr != NULL)
                fclose(min_ptr);
            fclose(min_ptr);
            free(max_path);
            free(min_path);
            return 0;
        }

        fscanf(max_ptr, "%d", &(fans->max));
        fscanf(min_ptr, "%d", &(fans->min));
        fans->step = (fans->max - fans->min) / ((settings->max_temp - settings->high_temp) * (settings->max_temp - settings->high_temp + 1) / 2);

        fclose(max_ptr);
        fclose(min_ptr);
        free(max_path);
        free(min_path);
        fans = fans->next;
    }

    return 1;
}

t_fan *load_fans(t_settings *settings) {
    short int cnt = 0;
    t_fan *head = (t_fan*)malloc(sizeof(*head)), *fan = head, *previous = NULL;
    FILE *fan_ptr = NULL;
    char *fan_path = NULL;

    for (;;) {
        // Check if n-th fan exists
        fan_path = concatenate_format("%s%d%s", FAN_PATH_BASE, ++cnt, FAN_PATH_MANUAL);
        fan_ptr = fopen(fan_path, "r");
        free(fan_path);
        if (fan_ptr == NULL)
            break;
        fclose(fan_ptr);

        // Set fan defaults
        fan->id = cnt;
        fan->speed = 0;
        fan->write_path = concatenate_format("%s%d%s", FAN_PATH_BASE, cnt, FAN_PATH_WRITE);
        fan->next = (t_fan*)malloc(sizeof(*fan));

        // Move to next fan
        previous = fan;
        fan = fan->next;
    }

    // No fans found
    if (previous == NULL) {
        free(head);
        return NULL;
    }

    // Set last fan in list
    previous->next = NULL;
    free(fan);

    if (!load_fans_default_speed(settings, head)) {
        free_fans(head);
        return NULL;
    }

    return head;
}

int set_fans_mode(t_fan *fans, enum fan_mode mode) {
    int state = 1;
    FILE *fan_ptr = NULL;
    char *fan_path = NULL;

    while (fans) {
        fan_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fans->id, FAN_PATH_MANUAL);
        fan_ptr = fopen(fan_path, "w");
        free(fan_path);

        if (fan_ptr == NULL) {
            state = 0;
            fans = fans->next;
            continue;
        }

        if (fprintf(fan_ptr, "%d\n", mode) == EOF)
            state = 0;
        fclose(fan_ptr);
        fans = fans->next;
    }

    return state;
}

void free_fan(t_fan *fan) {
    free(fan->write_path);
    free(fan);
}

int set_fan_speed(t_fan *fan, int speed) {
    if (fan->speed != speed) {
        fan->speed = speed;
        FILE *fan_ptr = fopen(fan->write_path, "w");

        if (fan_ptr == NULL)
            return 0;
        if (fprintf(fan_ptr, "%d\n", speed) == EOF) {
            fclose(fan_ptr);
            return 0;
        }

        fclose(fan_ptr);
    }
    return 1;
}
