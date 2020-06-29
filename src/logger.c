/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#include "logger.h"
#include "helper.h"


#define LOGGER_DATE_FORMAT "%b %d %H:%M:%S"
#define LOGGER_DATE_LENGTH 16


static const char* logger_level_strings[5] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "UNKNOWN"
};

// TODO: what to use when unknown level
static const int logger_syslog_level[5] = {
    LOG_ERR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_INFO
};


static int logger_get_time(int destination_size, char *destination) {
    time_t raw_time = 0;
    struct tm *time_info = NULL;

    errno = 0;
    raw_time = time(NULL);
    if (errno != 0)
        return 0;

    time_info = localtime(&raw_time);
    if (!time_info)
        return 0;

    if (strftime(destination, destination_size, LOGGER_DATE_FORMAT, time_info) == 0)
        return 0;

    return 1;
}


void logger_log(int level, const char *format, ...) {
    va_list ap;
    char *message = NULL, time_buffer[LOGGER_DATE_LENGTH], *string = NULL;

    // TODO: HANDLE ERROR
    if (level < LOG_L_ERROR || level > LOG_L_DEBUG)
        level = LOG_L_UNKNOWN;

    if (!logger_get_time(sizeof(time_buffer), time_buffer))
        strncpy(time_buffer, "??? ?? ??:??:??", sizeof(time_buffer));

    va_start(ap, format);
    message = concatenate_format_v(format, ap);
    va_end(ap);
    if (!message) {
        printf("Error.\n");
        return;
        // TODO: HANDLE ERROR
    }
    
    string = concatenate_format("%s %-7s %s", time_buffer, logger_level_strings[level], message);
    if (!string) {
        printf("Error.\n");
        free(message);
        return;
        // TODO: HANDLE ERROR
    }

    printf("%s\n", string);
    // TODO: Pass to proper function

    free(message);
    free(string);
    return;
}
