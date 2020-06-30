/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#include "logger.h"
#include "helper.h"

#define LOGGER_DATE_FORMAT "%b %d %H:%M:%S"
#define LOGGER_DATE_LENGTH 16


// Default logger settings
static struct {
    int type;
    FILE *log_file;
} logger = {
    .type = LOG_T_STD,
    .log_file = NULL
};


static const char* logger_level_strings[4] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};


// TODO: what to use when unknown level
static const int logger_level_syslog[4] = {
    LOG_ERR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
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


static void logger_print_std(int level, char *full_string) {
    // Here, as logger, we cannot really do much with I/O errors
    switch (level) {
        case LOG_L_ERROR:
            fprintf(stderr, "%s\n", (full_string) ? full_string : "ERROR LOGGING MESSAGE");
            fflush(stderr);
            break;
        default:
            fprintf(stdout, "%s\n", (full_string) ? full_string : "ERROR LOGGING MESSAGE");
            fflush(stdout);
            break;
    }
}


static void logger_print_file(char *full_string) {
    // Here, as logger, we cannot really do much with I/O errors
    fprintf(logger.log_file, "%s\n", (full_string) ? full_string : "ERROR LOGGING MESSAGE");
    fflush(logger.log_file);
}


static char* logger_construct_full_string(int level, const char *format, va_list ap) {
    char *message = NULL, *full_string = NULL, time_buffer[LOGGER_DATE_LENGTH];

    // Get log message time
    if (!logger_get_time(sizeof(time_buffer), time_buffer))
        strncpy(time_buffer, "??? ?? ??:??:??", sizeof(time_buffer));

    // Construct logged message
    message = concatenate_format_v(format, ap);
    if (!message)
        return NULL;

    // Construct full log message including time, level and message
    full_string = concatenate_format("%s %-7s: %s", time_buffer, logger_level_strings[level], message);
    if (!full_string) {
        free(message);
        return NULL;
    }

    free(message);
    return full_string;
}


int logger_set_type(int type, char* file_path) {
    if (type < LOG_T_STD || type > LOG_T_FILE)
        return 0;

    // Close previous log
    switch (logger.type) {
        case LOG_T_FILE:
            if (logger.log_file) {
                fclose(logger.log_file);
                logger.log_file = NULL;
            }
            break;
        case LOG_T_SYS:
            closelog();
            break;
        default:
            break;
    }

    // Setup log file
    if (type == LOG_T_FILE) {
        if (file_path == NULL)
            return 0;
        logger.log_file = fopen(file_path, "a");
        if (!logger.log_file)
            return 0;
    }

    // Setup syslog
    if (type == LOG_T_SYS)
        openlog("macfand", LOG_PID, LOG_DAEMON);

    logger.type = type;

    return 1;
}


void logger_log(int level, const char *format, ...) {
    va_list ap;
    char *full_string = NULL;

    va_start(ap, format);

    if (level < LOG_L_ERROR || level > LOG_L_DEBUG)
        level = LOG_L_ERROR;

    if (logger.type == LOG_T_SYS) {
        vsyslog(logger_level_syslog[level], format, ap);
        va_end(ap);
        return;
    }
    
    full_string = logger_construct_full_string(level, format, ap);
    va_end(ap);

    switch (logger.type) {
        case LOG_T_STD:
            logger_print_std(level, full_string);
            break;
        case LOG_T_FILE:
            logger_print_file(full_string);
            break;
    }

    if (full_string)
        free(full_string);
}


void logger_exit(void) {
    logger_log(LOG_L_INFO, "%s", "Shutting down");
    // Here, as logger, we cannot really do much with I/O errors
    if (logger.log_file)
        fclose(logger.log_file);
    if (logger.type == LOG_T_SYS)
        closelog();
}
