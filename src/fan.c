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

int load_fans_defaults(const t_settings *settings, t_node *fans) {
    char *max_path = NULL, *min_path = NULL;
    FILE *max_ptr = NULL, *min_ptr = NULL;
    t_fan *fan = NULL;

    while (fans) {

        fan = fans->data;

        max_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fan->id, FAN_PATH_MAX);
        if (!max_path)
            return 0;

        min_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fan->id, FAN_PATH_MIN);
        if (!min_path) {
            free(max_path);
            return 0;
        }

        max_ptr = fopen(max_path, "r");
        if (!max_ptr) {
            free(max_path);
            free(min_path);
            return 0;
        }

        min_ptr = fopen(min_path, "r");
        if (!min_ptr) {
            free(max_path);
            free(min_path);
            // Even if fclose() fails, we return 0
            fclose(max_ptr);
            return 0;
        }

        // Read max and min speed of fan
        if (fscanf(max_ptr, "%d", &(fan->max)) != 1 || fscanf(min_ptr, "%d", &(fan->min)) != 1) {
            free(max_path);
            free(min_path);
            fclose(max_ptr);
            fclose(min_ptr);
            return 0;
        }

        // Calculate size of one unit of fan speed change
        fan->step = (fan->max - fan->min) / ((settings->max_temp - settings->high_temp) * (settings->max_temp - settings->high_temp + 1) / 2);

        if (fclose(max_ptr) == EOF) {
            free(max_path);
            free(min_path);
            return 0;
        }

        if (fclose(min_ptr) == EOF) {
            free(max_path);
            free(min_path);
            return 0;
        }

        // Probably too extreme to exit after failing to close the file
        
        free(max_path);
        free(min_path);
        
        fans = fans->next;
    }

    return 1;
}

t_node *load_fans(const t_settings *settings) {
    int cnt = 0;
    t_node *head = (t_fan*)malloc(sizeof(*head));
    t_fan *fan = NULL;
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

    if (!load_fans_defaults(settings, head)) {
        free_list(head, (void (*)(void *))&free_fan);
        return NULL;
    }

    return head;
}

int set_fans_mode(t_node *fans, const enum fan_mode mode) {
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

int set_fan_speed(t_fan *fan, const int speed) {
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
