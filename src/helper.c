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
#include <errno.h>
#include <limits.h>

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

    // Prepare memory for string
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


size_t get_line_until(const char *line, const char delimeter, char **destination, size_t *destination_size) {
    size_t cnt = 0;

    if (!line)
        return 0;

    // Prepare buffer
    if (*destination_size == 0) 
        *destination_size = 32;
    if (!(*destination))
        *destination = (char*)malloc(*destination_size * sizeof(*destination));
    if (!(*destination))
        return -1;

    while (*line) {
        // Resize buffer
        if (cnt == *destination_size) {
            *destination_size <<= 1;
            if (*destination_size > 8192)
                return -1;
            *destination = (char*)realloc(*destination, *destination_size * sizeof(*destination));
            if (!(*destination))
                return -1;
        }

        if (*line == delimeter || *line == '\n')
            break;
        (*destination)[cnt++] = *line;
        line++;
    }

    (*destination)[cnt] = '\0';

    return cnt;
}


int get_int_from_string(char *string, int *destination) {
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
