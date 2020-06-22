/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_CONTROL_H_fsdfdsfsdf
#define MACFAND_CONTROL_H_fsdfdsfsdf

#include "config.h"
#include "fan.h"
#include "monitor.h"

/**
 * 
 */
void start_control(t_settings *settings, t_fan *fans, t_monitor *monitors);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf