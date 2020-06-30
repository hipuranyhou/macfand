/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_DAEMONIZE_H_dhsafkjjkf
#define MACFAND_DAEMONIZE_H_dhsafkjjkf

/**
 * @brief Sets the termination flag.
 * Sets the termination flag to 1 for exiting out of main control loop when signal is catched.
 * @param[in] sig Catched signal number.
 */
static void set_termination_flag(int sig);

/**
 * @brief Wrapper for all signal() calls;
 * Wrapper for all singal() calls for all signals we want to register.
 */
static void prepare_signals(void);

/**
 * @brief Daemonizes macfand.
 * Registers all signals, forks twice to daemonize macfand, closes all open file descriptors, opens syslog for macfand
 * and prints PID of daemon to /run/macfand.pid .
 */
void daemonize(void);

#endif //MACFAND_DAEMONIZE_H_dhsafkjjkf