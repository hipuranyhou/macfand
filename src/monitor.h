/**
 * macfand - hipuranyhou - 25.01.2021
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
 * @brief Struct holding all monitor ids.
 * Struct holding all monitor ids, which are monitor and hwmon.
 */
struct mon_id {
    int mon;
    int hw;
};

/**
 * @brief Struct holding all monitor temperatures.
 * Struct holding all monitor temperatures, which are real (current) and max.
 */
struct mon_temp {
    int real;
    int max;
};

/**
 * @brief Struct holding all monitor paths.
 * Struct holding all monitor paths, which are read and max.
 */
struct mon_path {
    char *rd;
    char *max;
};

/**
 * @brief Holds information about temperature monitor.
 * Struct holding id and hwmon entry id, current temperature and max temperature, path for reading temperature from
 * given monitor and its label.
 */
typedef struct mon {
    char            *lbl;
    struct mon_path path;
    struct mon_id   id;
    struct mon_temp temp;
} t_mon;

/**
 * @brief Constructs linked list of system temperature monitors.
 * Constructs generic linked list of all system temperature monitors. For each monitor sets its ids, 
 * current temperature to 0, temperature reading path and from appropiate system files loads 
 * its label and max temperature.
 * @return t_node* NULL on error, pointer to head of generic linked list of temperature monitors otherwise.
 */
t_node *mons_load(void);

/**
 * @brief Gets the current system temperature.
 * Gets the current system temperature, which is the highest value from current temperatures of all system monitors.
 * @param[in] monitors Pointer to head of linked list of temperature monitors.
 * @return int settings_get_value(SET_TEMP_HIGH) if reading at least one temperature failed, 
 * current system temperature otherwise.
 */
int mons_read_temp(t_node *mons);

/**
 * @brief Gets the system max allowed temperature
 * Gets the system max allowed temperature which is the lowest value of monitor.temp_max amongst all
 * system temperature monitors.
 * @param[in] monitors Pointer to head of linked list of temperature monitors.
 * @return int -1 on error, system max temperature otherwise.
 */
int mons_read_temp_max(const t_node *mons);

/**
 * @brief Frees memory for given monitor.
 * Calls free() on members of monitor if they are not NULL.
 * @param[in] monitor Pointer to temperature monitor.
 */
void mon_free(t_mon *mon, int self);

/**
 * @brief Prints info about monitor.
 * Prints formated information (all values of t_mon) about given monitor to stdout.
 * @param[in] monitor Pointer to temperature monitor.
 */
void mon_print(const t_mon *const mon, FILE *const file);

#endif //MACFAND_MONITOR_H_fajkdsfbua
