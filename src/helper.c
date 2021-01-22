/**
 * macfand - hipuranyhou - 22.01.2021
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
#include <ctype.h>

#include "helper.h"

/**
 * @brief Concatenates values into a string
 * Concatenates values in ap into a string with given format. Allocates memory which has to be freed by caller.
 * @param[in] fmt Format of constructed string.
 * @param[in] ap  Values to be concatenated into a string.
 * @return char* NULL on error, pointer to concatenated string otherwise (has to be freed!)
 */
static char* v_concat_fmt(const char* fmt, va_list ap);


static char* v_concat_fmt(const char* fmt, va_list ap) {
    va_list ap_len;
    int     len     = 0;
    int     vsn_ret = 0;
    char    *str    = NULL;

    // Get length of string
    va_copy(ap_len, ap);
    len = vsnprintf(NULL, 0, fmt, ap_len);
    va_end(ap_len);
    if (len < 0)
        return NULL;
    len++;

    // Prepare memory for string
    str = (char*)malloc(len);
    if (!str)
        return NULL;

    // Concatenate
    vsn_ret = vsnprintf(str, len, fmt, ap);
    if (vsn_ret > len) {
        if (str)
            free(str);
        return NULL;
    }

    return str;
}


char* concat_fmt(const char* fmt, ...) {
    va_list ap;
    char    *str = NULL;

    va_start(ap, fmt);
    str = v_concat_fmt(fmt, ap);
    va_end(ap);

    return str;
}


ssize_t get_word_until(const char *str, const char delim, char **dest, size_t *const dest_size) {
    size_t len = 0;

    if (!str || (*dest && *dest_size == 0) || (!(*dest) && *dest_size != 0))
        return -1;

    // Prepare buffer
    if (!(*dest) && *dest_size == 0) {
        *dest_size = 32;
        *dest = (char*)malloc(*dest_size * sizeof(*dest));
        if (!(*dest))
            return -1;
    }

    while (*str) {

        // Resize buffer
        if (len == *dest_size - 1) {
            *dest_size <<= 1;
            if (*dest_size > 4096)
                return -1;
            *dest = (char*)realloc(*dest, *dest_size * sizeof(*dest));
            if (!(*dest))
                return -1;
        }

        if (*str == delim || *str == '\n')
            break;
            
        (*dest)[len] = *str;
        len++;
        str++;
    }

    (*dest)[len] = '\0';

    return len;
}


int str_to_int(const char *const str, int *const dest, int base, char *const inv) {
    long l    = 0;
    char *end = NULL;

    if (!str || !isdigit(*str) || !dest || base < 2 || base > 36)
        return -1;

    errno = 0;
    l = strtol(str, &end, base);

    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return -1;
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return -1;

    *dest = l;

    if (inv)
        *inv = *end;

    if (*end != '\0')
        return 0;
    else
        return 1;
}


int max(const int a, const int b) {
    return (a > b) ? a : b;
}


int min(const int a, const int b) {
    return (a < b) ? a : b;
}
