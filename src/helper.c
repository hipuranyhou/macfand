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
#include <limits.h>
#include <errno.h>

#include "helper.h"


char* concatenate_format_v(const char* format, va_list ap) {
    va_list ap_len;
    int len = 0, vsn_ret = 0;
    char *string = NULL;

    // Get length of string
    va_copy(ap_len, ap);
    len = vsnprintf(NULL, 0, format, ap_len);
    va_end(ap_len);
    if (len < 0)
        return NULL;

    

    string = (char*)malloc(len + 1);
    if (!string)
        return NULL;

    // Concatenate
    vsn_ret = vsnprintf(string, len + 1, format, ap);

    if (vsn_ret > len) {
        if (string)
            free(string);
        return NULL;
    }

    return string;
}


char* concatenate_format(const char* format, ...) {
    va_list ap;
    char *string = NULL;

    va_start(ap, format);
    string = concatenate_format_v(format, ap);
    va_end(ap);
    
    return string;
}


int convert_valid_int(char *string, int *destination) {
    char *tmp_str;
    long int tmp_val = 0;

    errno = 0;
    tmp_val = strtol(string, &tmp_str, 10);

    if (tmp_str == string || *tmp_str != '\0' || tmp_val < INT_MIN || tmp_val > INT_MAX)
        return 0;

    *destination = (int)tmp_val;
    return 1;
}


int max(const int a, const int b) {
    return (a > b) ? a : b;
}


int min(const int a, const int b) {
    return (a < b) ? a : b;
}