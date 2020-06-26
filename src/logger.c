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

#include "logger.h"
#include "helper.h"

static struct log_event {
    const char *format;
};

void logger_log(const enum log_level level, const char *format, ...) {
    va_list ap;
    char *string = NULL;

    // TODO: HANDLE ERROR
    if (level < LOG_L_ERROR || level > LOG_L_DEBUG)
        return;

    va_start(ap, format);
    char *test = concatenate_format_v(format, ap);
    va_end(ap);

    if (!test)
        printf("Error.\n");
    else {
        printf("%s\n", test);
        free(test);
    }

    return;
}