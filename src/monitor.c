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
#include "settings.h"
#include "logger.h"

#define MONITOR_PATH_BASE "/sys/devices/platform/coretemp.0/hwmon/hwmon"
#define MONITOR_PATH_READ "_input"
#define MONITOR_PATH_MAX "_max"
#define MONITOR_PATH_LABEL "_label"
#define MONITOR_PATH_FORMAT "%s%d/temp%d%s"

/**
 * @brief Loads label of monitor.
 * Loads label of given monitor from appropriate system file.
 * @param[in,out]  monitor  Pointer to temperature monitor.
 * @return int 0 on error, 1 on success.
 */
static int monitor_load_label(t_monitor *monitor);

/**
 * @brief Loads max temperature of monitor.
 * Loads max temperature of given monitor from appropriate system file.
 * @param[in,out]  monitor  Pointer to temperature monitor.
 * @return int 0 on error, 1 on success.
 */
static int monitor_load_max_temp(t_monitor *monitor);

/**
 * @brief Loads default values for given monitor.
 * Loads max temperature and label from appropiate files for given monitor and constructs its reading path.
 * @param[in,out]  monitor  Pointer to temperature monitor.
 * @return int 0 on error, 1 on success.
 */
static int monitor_load_defaults(t_monitor *monitor);

/**
 * @brief Checks if monitor given monitor exists.
 * Check whether monitor with given id for given hwmon entry id exists by trying to opening its 
 * temperature reading path.
 * @param[in]  id_hw  hwmon entry id.
 * @param[in]  id_mon  id of temperature monitor for given hwmon entry.
 * @return int -1 on error, 0 if does not exist, 1 if exists.
 */
static int monitor_id_exists(const int id_hw, const int id_mon);


static int monitor_load_label(t_monitor *monitor) {
    char *monitor_path_label = NULL;
    FILE *monitor_file_label = NULL;
    int getline_return = 0;
    size_t buffer_size = 0;

    if (!monitor)
        return 0;

    monitor->label = NULL;

    monitor_path_label = concatenate_format(MONITOR_PATH_FORMAT, MONITOR_PATH_BASE, monitor->id_hw, monitor->id, MONITOR_PATH_LABEL);
    if (!monitor_path_label)
        return 0;

    monitor_file_label = fopen(monitor_path_label, "r");
    free(monitor_path_label);
    if (!monitor_file_label)
        return 0;

    getline_return = getline(&(monitor->label), &buffer_size, monitor_file_label);
    if (getline_return == -1) {
        fclose(monitor_file_label);
        return 0;
    }

    // (getline_return-1) because line ends with "0x0A"
    monitor->label[getline_return-1] = '\0';

    // We silently ignore return value of fclose() when just reading from it
    fclose(monitor_file_label);
    return 1;
}


static int monitor_load_max_temp(t_monitor *monitor) {
    char *monitor_path_max = NULL;
    FILE *monitor_file_max = NULL;

    if (!monitor)
        return 0;

    monitor_path_max = concatenate_format(MONITOR_PATH_FORMAT, MONITOR_PATH_BASE, monitor->id_hw, monitor->id, MONITOR_PATH_MAX);
    if (!monitor_path_max)
        return 0;

    monitor_file_max = fopen(monitor_path_max, "r");
    free(monitor_path_max);
    if (!monitor_file_max)
        return 0;

    if (fscanf(monitor_file_max, "%d", &(monitor->temp_max)) != 1) {
        fclose(monitor_file_max);
        return 0;
    }

    monitor->temp_max /= 1000; 

    // We silently ignore return value of fclose() when just reading from it
    fclose(monitor_file_max);
    return 1;
}

static int monitor_load_defaults(t_monitor *monitor) {
    if (!monitor)
        return 0;

    if (!monitor_load_max_temp(monitor)) {
        logger_log(LOG_L_DEBUG, "%s %d (hwmon%d)", "Unable to load max temperature of monitor", monitor->id, monitor->id_hw);
        return 0;
    }

    monitor->path_read = concatenate_format(MONITOR_PATH_FORMAT, MONITOR_PATH_BASE, monitor->id_hw, monitor->id, MONITOR_PATH_READ);
    if (!monitor->path_read)
        return 0;

    if (!monitor_load_label(monitor)) {
        logger_log(LOG_L_DEBUG, "%s %d (hwmon%d)", "Unable to load label of monitor", monitor->id, monitor->id_hw);
        return 0;
    }

    return 1;
}


static int monitor_id_exists(const int id_hw, const int id_mon) {
    FILE *monitor_file_read = NULL;
    char *monitor_path_read = NULL;

    monitor_path_read = concatenate_format(MONITOR_PATH_FORMAT, MONITOR_PATH_BASE, id_hw, id_mon, MONITOR_PATH_READ);
    if (!monitor_path_read)
        return -1;

    monitor_file_read = fopen(monitor_path_read, "r");
    free(monitor_path_read);
    if (!monitor_file_read)
        return 0;

    // We silently ignore return value of fclose() when just checking if the file exists
    fclose(monitor_file_read);
    return 1;
}


t_node *monitors_load(void) {
    int id_mon = 0, monitor_exists = 0;
    t_monitor *monitor = NULL;
    t_node *monitors = NULL;

    monitor = (t_monitor*)malloc(sizeof(*monitor));
    if (!monitor)
        return NULL;

    monitor->temp_current = 0;

    for (int id_hw = 0; id_hw < 16; ++id_hw) {
        for (;;) {
            monitor->id_hw = id_hw;
            monitor->id = ++id_mon;
            monitor_exists = monitor_id_exists(id_hw, id_mon);

            if (monitor_exists == 0)
                break;

            if (monitor_exists == -1                        ||
                !monitor_load_defaults(monitor)             ||
                !list_push_front(&monitors, monitor, sizeof(*monitor)))
            {
                list_free(monitors, (void (*)(void *))monitor_free);
                monitors = NULL;
                free(monitor);
                break;
            }
        }
    }

    free(monitor);
    return monitors;
}


int monitors_get_temp(const t_node *monitors) {
    int temp = 0;
    FILE *monitor_file_read = NULL;
    t_monitor *monitor = NULL;

    if (!monitors)
        return 100;

    while (monitors) {
        monitor = monitors->data;

        monitor_file_read = fopen(monitor->path_read, "r");
        if (monitor_file_read == NULL) {
            monitors = monitors->next;
            continue;
        }

        if (fscanf(monitor_file_read, " %d", &(monitor->temp_current)) == EOF) {
            fclose(monitor_file_read);
            monitors = monitors->next;
            continue;
        }

        // We silently ignore return value of fclose() when just checking if the file exists
        fclose(monitor_file_read);
        temp = (monitor->temp_current > temp) ? monitor->temp_current : temp;
        monitors = monitors->next;
    }

    // If failed to load at least one temperature, crank up the fans
    if (temp == 0) {
        logger_log(LOG_L_DEBUG, "%s", "Unable to get current system temperature, using 100°C");
        return settings_get_value(SET_TEMP_MAX);
    }

    return (temp / 1000);
}


int monitors_get_max_temp(const t_node *monitors) {
    int temp_max = 10000;
    t_monitor *monitor = NULL;

    if (!monitors)
        return -1;

    while (monitors) {
        monitor = monitors->data;
        temp_max = (temp_max < monitor->temp_max) ? temp_max : monitor->temp_max;
        monitors = monitors->next;
    }

    return (temp_max != 10000) ? temp_max : -1;
}


void monitor_free(t_monitor *monitor) {
    if (!monitor)
        return;
    if (monitor->path_read)
        free(monitor->path_read);
    if (monitor->label)
        free(monitor->label);
    free(monitor);
}


void monitor_print(const t_monitor *monitor) {
    if (!monitor)
        return;
    printf("Monitor %d - %s\n", monitor->id, monitor->label);
    printf("Current temp: %d°C  Max temp: %d°C\n", monitor->temp_current, monitor->temp_max);
    printf("Read: %s\n", monitor->path_read);
}