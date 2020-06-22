/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_CONTROL_H_fsdfdsfsdf
#define MAC_FAN_CONTROL_CONTROL_H_fsdfdsfsdf

#include "config.h"
#include "fan.h"
#include "monitor.h"

/**
 * 
 */
void start_control(t_settings *settings, t_fan *fans, t_monitor *monitors);

#endif //MAC_FAN_CONTROL_CONTROL_H_fsdfdsfsdf