/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

/*
* daemonize.c
* This example daemonizes a process.
* This is an answer to the stackoverflow question:
* https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
* Fork this code: https://github.com/pasce/daemon-skeleton-linux-c
* Read about it: https://nullraum.net/how-to-create-a-daemon-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include "daemonize.h"

extern volatile int termination_flag;

void set_termination_flag(int sig) {
    syslog(LOG_NOTICE, "Resetting fans to automatic mode and terminating mac_fan_control.");
    termination_flag = 1;
}

void prepare_signals(void) {
    // Prepare all signals after which we terminate
    signal(SIGABRT, set_termination_flag);
    signal(SIGINT, set_termination_flag);
    signal(SIGQUIT, set_termination_flag);
    signal(SIGTERM, set_termination_flag);
}

void daemonize(void) {

    FILE *pid_file = NULL;

    pid_t pid = 0;
    int fd;

    // Fork off the parent
    pid = fork();

    // Error during forking
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Terminate parent
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Child becomes leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Handle signals
    prepare_signals();

    // Fork off second time
    pid = fork();

    // Error during forking
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Terminate parent
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Set new file permissions
    umask(0);

    // Change to root directory
    chdir("/");

    // Close all open file descriptors
    for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
        close(fd);

    // Open the log file
    openlog("mac_fan_control", LOG_PID, LOG_DAEMON);

    // Write PID file
    pid_file = fopen("/run/macfand.pid", "w+");
    if (pid_file == NULL) {
        syslog(LOG_ERR, "Unable to open pid file.");
        return;
    }
    if (fprintf(pid_file, "%d\n", getpid()) == EOF)
        syslog(LOG_ERR, "Unable to write pid file.");
    fclose(pid_file);
}
