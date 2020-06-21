/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_DAEMONIZE_H_dhsafkjjkf
#define MAC_FAN_CONTROL_DAEMONIZE_H_dhsafkjjkf

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

#endif //MAC_FAN_CONTROL_DAEMONIZE_H_dhsafkjjkf