//
// Created by hipuranyhou on 21.06.20.
//

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