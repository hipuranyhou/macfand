/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdarg.h>
#include <stdlib.h>

#include "helper.h"

char* concatenate_format(const char* format, ...) {
    va_list ap;
    int cnt = 0;
    char* string = NULL;

    // Get length of string
    va_start(ap, format);
    cnt = vsnprintf(NULL, 0, format, ap);
    va_end(ap);

    if (cnt < 0)
        return NULL;
    string = (char*)malloc(cnt + 1);

    // Concatenate
    va_start(ap, format);
    vsnprintf(string, cnt + 1, format, ap);
    va_end(ap);

    return string;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}