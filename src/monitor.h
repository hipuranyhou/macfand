/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_MONITOR_H_fajkdsfbua
#define MACFAND_MONITOR_H_fajkdsfbua

#include <stdio.h>

#include "linked.h"

/**
 * @brief Holds information about temperature monitor.
 * Struct holding id and hwmon entry id, current temperature and max temperature, path for reading temperature from
 * given monitor and its label.
 */
typedef struct monitor {
    int id;
    int id_hw;
    int temp_current;
    int temp_max;
    char *path_read;
    char *label;
} t_monitor;

/**
 * @brief Constructs linked list of system temperature monitors.
 * Constructs generic linked list of all system temperature monitors for up to 16 hwmon entries and unlimited entries
 * in hwmon subfolders. For each monitor sets its ids, current temperature to 0, temperature reading path and from 
 * appropiate system files loads its label and max temperature.
 * @return t_node* NULL on error, pointer to head of generic linked list of temperature monitors otherwise.
 */
t_node *monitors_load(void);

/**
 * @brief Gets the current system temperature.
 * Gets the current system temperature, which is the highest value from current temperatures of all system monitors.
 * @param[in]  monitors  Pointer to head of linked list of temperature monitors.
 * @return int 100 if reading at least one temperature failed, current system temperature otherwise.
 */
int monitors_get_temp(const t_node *monitors);

/**
 * @brief Gets the system max allowed temperature
 * Gets the system max allowed temperature which is the lowest value of monitor.temp_max amongst all
 * system temperature monitors.
 * @param[in]  monitors   Pointer to head of linked list of temperature monitors.
 * @return int -1 on error, system max temperature otherwise.
 */
int monitors_get_temp_max(const t_node *monitors);

/**
 * @brief Frees memory for given monitor.
 * Calls free() on members of monitor if they are not NULL.
 * @param[in] monitor  Pointer to temperature monitor.
 */
void monitor_free(t_monitor *monitor);

/**
 * @brief Prints info about monitor.
 * Prints formated information (all values of t_monitor) about given monitor to stdout.
 * @param[in]  monitor  Pointer to temperature monitor.
 */
void monitor_print(const t_monitor *monitor, FILE *stream);

#endif //MACFAND_MONITOR_H_fajkdsfbua
