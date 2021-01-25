/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_WIDGET_H_weqeurhqiu
#define MACFAND_WIDGET_H_weqeurhqiu

#include "linked.h"

/**
 * @brief Writes speed widget file.
 * Writes speed of each system fan to widget file.
 * @param[in]  fans  Pointer to head of linked list of system fans.
 */
void wgt_write(const t_node *fans);

#endif //MACFAND_WIDGET_H_weqeurhqiu
