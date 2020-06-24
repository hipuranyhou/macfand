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
 * @brief 
 * 
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
 * @brief 
 * 
 * @param monitor 
 * @return int 
 */
int monitor_load_label(t_monitor *monitor);

/**
 * @brief 
 * 
 * @param monitor 
 * @return int 
 */
int monitor_load_max_temp(t_monitor *monitor);

/**
 * @brief 
 * 
 * @param monitor 
 * @return int 
 */
int monitor_load_defaults(t_monitor *monitor);

/**
 * @brief 
 * 
 * @param cnt_hw 
 * @param cnt_temp 
 * @return int 
 */
int monitor_id_exists(const int cnt_hw, const int cnt_temp);

/**
 * @brief 
 * 
 * @param settings 
 * @return t_node* 
 */
t_node *monitors_load(void);

/**
 * @brief Get the current temp object
 * 
 * @param monitors 
 * @return int 
 */
int monitors_get_temp(const t_node *monitors);

/**
 * @brief 
 * 
 * @param monitor 
 */
void monitor_free(t_monitor *monitor);

/**
 * @brief 
 * 
 */
void monitor_print(const t_monitor *monitor);

#endif //MACFAND_MONITOR_H_fajkdsfbua
