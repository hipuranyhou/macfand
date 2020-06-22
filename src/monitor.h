/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_MONITOR_H_fajkdsfbua
#define MAC_FAN_CONTROL_MONITOR_H_fajkdsfbua

#include "config.h"

/**
 * Struct holding all information about one temp monitor.
 */
typedef struct monitor {
    short int id;
    int temp;
    char *read_path;
    struct monitor *next;
} t_monitor;


/**
 * 
 * @param[in]
 * @param[in]
 * @param[in]
 */
void set_max_temp(t_settings *settings, int cnt_hw, int cnt_mon);

/**
 * 
 * @param[in]
 * @paran[in]
 * @return
 */
int monitor_exists(int cnt_hw, int cnt_temp);

/**
 *
 * @return
 */
t_monitor *load_monitors(t_settings *settings);

/**
 * Frees all monitors in linked list.
 * @param[in] monitors Pointer to head of linked list of all monitors.
 */
void free_monitors(t_monitor *monitors);

/**
 *
 * @param monitors
 * @return
 */
int get_current_temp(t_monitor *monitors);

#endif //MAC_FAN_CONTROL_MONITOR_H_fajkdsfbua
