/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_MONITOR_H_fajkdsfbua
#define MACFAND_MONITOR_H_fajkdsfbua

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
int monitors_get_max_temp(const t_node *monitors);

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
void monitor_print(const t_monitor *monitor);

#endif //MACFAND_MONITOR_H_fajkdsfbua
