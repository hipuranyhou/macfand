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
#include <dirent.h>

#include "monitor.h"
#include "helper.h"

#define MONITOR_PATH_BASE "/sys/devices/platform/coretemp.0/hwmon/hwmon"
#define MONITOR_PATH_READ "_input"
#define MONITOR_PATH_MAX "_max"
#define MONITOR_FORMAT "%s%d/temp%d%s"

void set_max_temp(t_settings *settings, int cnt_hw, int cnt_mon) {
    int max_temp = 0;
    FILE *max_ptr = NULL;
    char *max_path = NULL;

    max_path = concatenate_format(MONITOR_FORMAT, MONITOR_PATH_BASE, cnt_hw, cnt_mon, MONITOR_PATH_MAX);
    max_ptr = fopen(max_path, "r");
    free(max_path);

    if (max_ptr != NULL) {
        if (fscanf(max_ptr, "%d", &max_temp) == EOF)
            max_temp = 0;
        max_temp /= 1000;
        settings->max_temp = (settings->max_temp > max_temp) ? settings->max_temp : max_temp;
        fclose(max_ptr);
    }
}

int monitor_exists(int cnt_hw, int cnt_mon) {
    FILE *read_ptr = NULL;
    char *read_path = NULL;

    read_path = concatenate_format(MONITOR_FORMAT, MONITOR_PATH_BASE, cnt_hw, cnt_mon, MONITOR_PATH_READ);
    read_ptr = fopen(read_path, "r");
    free(read_path);

    if (read_ptr == NULL)
        return 0;

    fclose(read_ptr);
    return 1;
}

t_monitor *load_monitors(t_settings *settings) {
    int cnt_mon = 0;
    t_monitor *head = (t_monitor*)malloc(sizeof(*head)), *monitor = head, *previous = NULL;

    for (int cnt_hw = 0; cnt_hw < 16; ++cnt_hw) {
        for (;;) {

            if (!monitor_exists(cnt_hw, ++cnt_mon))
                break;

            set_max_temp(settings, cnt_hw, cnt_mon);
            
            // Set monitor defaults
            monitor->id = cnt_mon;
            monitor->read_path = concatenate_format(MONITOR_FORMAT, MONITOR_PATH_BASE, cnt_hw, cnt_mon, MONITOR_PATH_READ);
            monitor->next = (t_monitor*)malloc(sizeof(*monitor));

            // Move to next monitor
            previous = monitor;
            monitor = monitor->next;
        }
    }

    // No monitors found
    if (previous == NULL) {
        free(head);
        return NULL;
    }

    // Set last monitor in list
    previous->next = NULL;
    free(monitor);
    return head;
}

void free_monitor(t_monitor *monitor) {
    free(monitor->read_path);
    free(monitor);
}

int get_current_temp(t_monitor *monitors) {
    int temp = 0, monitor_temp;
    FILE *temp_ptr = NULL;

    while (monitors) {
        temp_ptr = fopen(monitors->read_path, "r");
        if (temp_ptr == NULL) {
            monitors = monitors->next;
            continue;
        }
        if (fscanf(temp_ptr, "%d", &monitor_temp) == EOF) {
            fclose(temp_ptr);
            monitors = monitors->next;
            continue;
        }
        fclose(temp_ptr);
        temp = (monitor_temp > temp) ? monitor_temp : temp;
        monitors = monitors->next;
    }

    // Return 100 if failed to load at least one temperature to crank up the fans
    return (temp == 0) ? 100 : (temp / 1000);
}