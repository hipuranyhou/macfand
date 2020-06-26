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
#define FAN_PATH_LABEL "_label"
#define FAN_PATH_FORMAT "%s%d%s"


int fan_load_label(t_fan *fan) {
    char *fan_path_label = NULL;
    FILE *fan_file_label = NULL;
    int getline_return = 0;
    size_t buffer_size = 0;

    if (!fan)
        return 0;

    fan->label = NULL;

    fan_path_label = concatenate_format(FAN_PATH_FORMAT, FAN_PATH_BASE, fan->id, FAN_PATH_LABEL);
    if (!fan_path_label)
        return 0;

    fan_file_label = fopen(fan_path_label, "r");
    free(fan_path_label);
    if (!fan_file_label)
        return 0;

    getline_return = getline(&(fan->label), &buffer_size, fan_file_label);
    if (getline_return == -1) {
        fclose(fan_file_label);
        return 0;
    }

    // (getline_return - 1) because line ends with "0x10_0x0A"
    fan->label[getline_return-1] = '\0';

    // We silently ignore return value of fclose() when just reading from it
    fclose(fan_file_label);
    return 1;
}


int fan_load_speed(t_fan *fan, int *destination, const char *speed) {
    char *fan_path_speed = NULL;
    FILE *fan_file_speed = NULL;

    if (!fan)
        return 0;

    fan_path_speed = concatenate_format(FAN_PATH_FORMAT, FAN_PATH_BASE, fan->id, speed);
    if (!fan_path_speed)
        return 0;

    fan_file_speed = fopen(fan_path_speed, "r");
    free(fan_path_speed);
    if (!fan_file_speed)
        return 0;

    if (fscanf(fan_file_speed, " %d", destination) != 1) {
        fclose(fan_file_speed);
        return 0;
    }

    // We silently ignore return value of fclose() when just reading from it
    fclose(fan_file_speed);
    return 1;
}


int fan_load_defaults(const t_settings *settings, t_fan *fan) {
    if (!fan || !settings)
        return 0;

    if (!fan_load_speed(fan, &(fan->min), FAN_PATH_MIN) || !fan_load_speed(fan, &(fan->max), FAN_PATH_MAX))
        return 0;

    // Calculate size of one unit of fan speed change
    fan->step = (fan->max - fan->min) / ((settings->temp_max - settings->temp_high) * (settings->temp_max - settings->temp_high + 1) / 2);

    fan->path_write = concatenate_format(FAN_PATH_FORMAT, FAN_PATH_BASE, fan->id, FAN_PATH_WRITE);
    if (!fan->path_write)
        return 0;

    fan->path_manual = concatenate_format(FAN_PATH_FORMAT, FAN_PATH_BASE, fan->id, FAN_PATH_MANUAL);
    if (!fan->path_manual)
        return 0;

    if (!fan_load_label(fan))
        return 0;

    return 1;
}


int fan_id_exists(const int id_fan) {
    FILE *fan_file_manual = NULL;
    char *fan_path_manual = NULL;

    fan_path_manual = concatenate_format(FAN_PATH_FORMAT, FAN_PATH_BASE, id_fan, FAN_PATH_MANUAL);
    if (!fan_path_manual)
        return -1;

    fan_file_manual = fopen(fan_path_manual, "r");
    free(fan_path_manual);
    if (fan_file_manual == NULL)
        return 0;

    // We silently ignore return value of fclose() when just reading from it
    fclose(fan_file_manual);
    return 1;
}


t_node *fans_load(const t_settings *settings) {
    int id_fan = 0, fan_exists = 0;
    t_fan *fan = NULL;
    t_node *fans = NULL;

    if (!settings)
        return NULL;

    fan = (t_fan *)malloc(sizeof(*fan));
    if (!fan)
        return NULL;

    fan->speed = 0;

    for (;;) {
        fan->id = ++id_fan;
        fan_exists = fan_id_exists(id_fan);

        if (fan_exists == 0)
            break;

        if (fan_exists == -1                            ||
            !fan_load_defaults(settings, fan)           ||
            !list_push_front(&fans, fan, sizeof(*fan)))
        {
            list_free(fans, (void (*)(void *))fan_free);
            fans = NULL;
            free(fan);
            break;
        }
    }

    free(fan);
    return fans;
}


int fans_set_mode(t_node *fans, const enum fan_mode mode) {
    int state = 1;
    FILE *fan_file_manual = NULL;
    t_fan *fan = NULL;

    if (!fans)
        state = 0;

    while (fans) {
        fan = fans->data;

        fan_file_manual = fopen(fan->path_manual, "w");
        if (!fan_file_manual) {
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


int fan_set_speed(t_fan *fan, const int speed) {
    FILE *fan_file_write = NULL;

    if (!fan)
        return 0;

    if (fan->speed == speed)
        return 1;

    fan_file_write = fopen(fan->path_write, "w");
    if (!fan_file_write)
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


void fan_free(t_fan *fan) {
    if (!fan)
        return;
    if (fan->path_write)
        free(fan->path_write);
    if (fan->path_manual)
        free(fan->path_manual);
    if (fan->label)
        free(fan->label);
    free(fan);
}


void fan_print(const t_fan *fan) {
    if (!fan)
        return;
    printf("Fan %d - %s\n", fan->id, fan->label);
    printf("Min speed: %d   Max speed: %d\n", fan->min, fan->max);
    printf("Speed: %d   Step: %d\n", fan->speed, fan->step);
    printf("Write: %s\n", fan->path_write);
    printf("Manual: %s\n", fan->path_manual);
    printf("\n");
}