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


int fan_load_defaults(const t_settings *settings, t_fan *fan, const int fan_cnt) {
    char *max_path = NULL, *min_path = NULL;
    FILE *max_ptr = NULL, *min_ptr = NULL;

    if (!fan || !settings)
        return 0;

    max_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fan_cnt, FAN_PATH_MAX);
    if (!max_path)
        return 0;

    min_path = concatenate_format("%s%d%s", FAN_PATH_BASE, fan_cnt, FAN_PATH_MIN);
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

    // Set max and min speed of fan
    if (fscanf(max_ptr, "%d", &(fan->max)) != 1 || fscanf(min_ptr, "%d", &(fan->min)) != 1) {
        free(max_path);
        free(min_path);
        fclose(max_ptr);
        fclose(min_ptr);
        return 0;
    }

    // Calculate size of one unit of fan speed change
    fan->step = (fan->max - fan->min) / ((settings->max_temp - settings->high_temp) * (settings->max_temp - settings->high_temp + 1) / 2);

    fan->id = fan_cnt;

    fan->path_write = concatenate_format("%s%d%s", FAN_PATH_BASE, fan_cnt, FAN_PATH_WRITE);
    if (!fan->path_write) {
        free(max_path);
        free(min_path);
        fclose(max_ptr);
        fclose(min_ptr);
        return 0;
    }

    fan->path_manual = concatenate_format("%s%d%s", FAN_PATH_BASE, fan_cnt, FAN_PATH_MANUAL);
    if (!fan->path_manual) {
        free(max_path);
        free(min_path);
        fclose(max_ptr);
        fclose(min_ptr);
        return 0;
    }

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
    
    free(max_path);
    free(min_path);

    return 1;
}


int fan_exists(const t_fan *fan) {
    FILE *fan_file = NULL;

    fan_file = fopen(fan->path_write, "r");
    if (fan_file == NULL)
        return 0;

    fclose(fan_file);
    return 1;
}


t_node *fans_load(const t_settings *settings) {
    int fan_cnt = 0;
    t_fan *fan = NULL;
    t_node *fans = NULL;

    if (!settings)
        return NULL;

    fan = (t_fan *)malloc(sizeof(*fan));
    if (!fan)
        return NULL;
    fan->speed = 0;

    for (;;) {
        if (!fan_load_defaults(settings, fan, ++fan_cnt)  ||
            !fan_exists(fan)                              || 
            !list_push_front(&fans, fan, sizeof(*fan))) 
        {
            list_free(fans, (void (*)(void *))&fan_free);
            fans = NULL;
            break;
        }
    }

    fan_free(fan);
    return fans;
}


int fans_set_mode(const t_node *fans, const enum fan_mode mode) {
    int state = 1;
    FILE *fan_file_manual = NULL;
    t_fan *fan = NULL;

    if (!fans)
        state = 0;

    while (fans) {
        fan = fans->data;

        fan_file_manual = fopen(fan->path_manual, "w");
        if (fan_file_manual == NULL) {
            state = 0;
            fans = fans->next;
            continue;
        }

        if (fprintf(fan_file_manual, "%d\n", mode) == EOF)
            state = 0;

        if (fclose(fan_file_manual) == EOF)
            state = 0;

        fans = fans->next;
    }

    return state;
}


void fan_free(t_fan *fan) {
    if (!fan)
        return;
    if (fan->path_write)
        free(fan->path_write);
    if (fan->path_manual)
        free(fan->path_manual);
    free(fan);
}


int fan_set_speed(t_fan *fan, const int speed) {
    FILE *fan_file_write = NULL;

    if (!fan)
        return 0;

    if (fan->speed == speed)
        return 1;

    fan_file_write = fopen(fan->path_write, "w");

    if (fan_file_write == NULL)
        return 0;

    if (fprintf(fan_file_write, "%d\n", speed) == EOF) {
        fclose(fan_file_write);
        return 0;
    }

    if (fclose(fan_file_write) == EOF)
        return 0;

    fan->speed = speed;

    return 1;
}
