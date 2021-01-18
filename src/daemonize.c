/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
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
#include <sys/types.h>
#include <sys/stat.h>

#include "daemonize.h"
#include "logger.h"


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

    // Write PID file
    pid_file = fopen("/run/macfand.pid", "w+");
    if (!pid_file) {
        logger_log(LOG_L_DEBUG, "%s", "Unable to open PID file");
        return;
    }
    if (fprintf(pid_file, "%d\n", getpid()) < 0)
        logger_log(LOG_L_DEBUG, "%s", "Unable to write PID file");
    if (fclose(pid_file) == EOF)
        logger_log(LOG_L_DEBUG, "%s", "Unable to close PID file");
}
