/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_DAEMONIZE_H_dhsafkjjkf
#define MACFAND_DAEMONIZE_H_dhsafkjjkf

/**
 * @brief Daemonizes macfand.
 * Registers all signals, forks twice to daemonize macfand, closes all open file descriptors, opens syslog for macfand
 * and prints PID of daemon to /run/macfand.pid .
 */
void daemonize(void);

#endif //MACFAND_DAEMONIZE_H_dhsafkjjkf