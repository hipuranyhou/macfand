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
 * 
 */
void set_termination_flag(int sig);

/**
 * 
 * 
 */
void prepare_signals(void);

/**
 * 
 * 
 */
void daemonize(void);

#endif //MACFAND_DAEMONIZE_H_dhsafkjjkf