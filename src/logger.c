/**
 * macfand - hipuranyhou - 25.01.2021
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
#include "settings.h"

#define LOG_TIME_FMT "%b %d %H:%M:%S"
#define LOG_TIME_LEN 16

/**
 * @brief Constructs time string for logging.
 * Constructs time string with format given in LOGGER_DATE_FORMAT into destination.
 * @param[in]  dest      Pointer to destination of time string.
 * @param[out] dest_size Size of buffer for time string.
 * @return int 0 on error, 1 on success.
 */
static int log_get_time(char *const dest, int dest_size);

/**
 * @brief Prints given string to std.
 * Prints given string to stderr for level LOG_L_ERROR and to stdout for other levels.
 * @param[in] lvl Level of message priority (one of enum log_level).
 * @param[in] msg String to be logged.
 */
static void log_print_std(int lvl, const char *const msg);

/**
 * @brief Prints given string to log file.
 * Prints given string to log file.
 * @param[in] msg String to be logged.
 */
static void log_print_file(const char *const msg);

/**
 * @brief Construct full logged string.
 * Constructs full logged string by getting current time, level string and message with given format
 * and concatenates these together. Allocates memory which has to be freed by caller.
 * @param[in] lvl Level of message priority (one of enum log_level).
 * @param[in] fmt Format of constructed message.
 * @param[in] ap  Values to be concatenated into a string.
 * @return char* NULL on error, pointer to full log string otherwise (has to be free by caller).
 */
static char* log_get_full_msg(int lvl, const char *const fmt, va_list ap);


/**
 * @brief Struct holding logger info.
 * Struct holding logger info with defaults set.
 */
static struct {
    int  type;
    FILE *file;
} logger = {
    .type = LOG_T_STD,
    .file = NULL
};


/**
 * @brief Message level strings.
 * Array holding all message level strings prepended to logged message.
 */
static const char* log_lvl_str[4] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};


/**
 * @brief Syslog level types
 * Array mapping logger levels to syslog levels.
 */
static const int log_lvl_sys[4] = {
    LOG_ERR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
};


static int log_get_time(char *const dest, int dest_size) {
    time_t    raw_time = 0;
    struct tm *tm      = NULL;

    errno = 0;
    raw_time = time(NULL);
    if (errno != 0)
        return 0;

    tm = localtime(&raw_time);
    if (!tm)
        return 0;

    if (strftime(dest, dest_size, LOG_TIME_FMT, tm) == 0)
        return 0;

    return 1;
}


static void log_print_std(int lvl, const char *const msg) {
    // Here, as logger, we cannot really do much with I/O errors
    switch (lvl) {
        case LOG_L_ERROR:
            fprintf(stderr, "%s\n", (msg) ? msg : "ERROR LOGGING MESSAGE");
            fflush(stderr);
            break;
        default:
            fprintf(stdout, "%s\n", (msg) ? msg : "ERROR LOGGING MESSAGE");
            fflush(stdout);
            break;
    }
}


static void log_print_file(const char *const msg) {
    // Here, as logger, we cannot really do much with I/O errors
    fprintf(logger.file, "%s\n", (msg) ? msg : "ERROR LOGGING MESSAGE");
    fflush(logger.file);
}


static char* log_get_full_msg(int lvl, const char *const fmt, va_list ap) {
    char *msg                = NULL;
    char *full_msg           = NULL;
    char time[LOG_TIME_LEN];

    // Get log message time
    if (!log_get_time(time, sizeof(*time) * LOG_TIME_LEN))
        strncpy(time, "??? ?? ??:??:??", sizeof(*time) * LOG_TIME_LEN);

    // Construct logged message
    msg = v_concat_fmt(fmt, ap);
    if (!msg)
        return NULL;

    // Construct full log message including time, level and message
    full_msg = concat_fmt("[%s] %-7s: %s", time, log_lvl_str[lvl], msg);

    free(msg);
    return full_msg;
}


int log_set_type(int type, const char *const path) {
    if (type < LOG_T_STD || type > LOG_T_FILE)
        return 0;

    // Close previous log
    switch (logger.type) {
        case LOG_T_FILE:
            if (logger.file) {
                fclose(logger.file);
                logger.file = NULL;
            }
            break;
        case LOG_T_SYS:
            closelog();
            break;
        default:
            break;
    }

    logger.type = type;

    // Setup log file
    if (type == LOG_T_FILE) {
        if (!path)
            return 0;
        logger.file = fopen(path, "a");
        if (!logger.file)
            return 0;
    }

    // Setup syslog
    if (type == LOG_T_SYS)
        openlog("macfand", LOG_PID, LOG_DAEMON);

    return 1;
}


void log_log(int lvl, const char *const fmt, ...) {
    va_list ap;
    char    *msg = NULL;

    // We log only errors when not in verbose mode
    if (lvl != LOG_L_ERROR && !set_get_int(SET_VERBOSE))
        return;

    va_start(ap, fmt);

    if (lvl < LOG_L_ERROR)
        lvl = LOG_L_ERROR;
    else if (lvl > LOG_L_DEBUG)
        lvl = LOG_L_DEBUG;
    

    if (logger.type == LOG_T_SYS) {
        vsyslog(log_lvl_sys[lvl], fmt, ap);
        va_end(ap);
        return;
    }
    
    msg = log_get_full_msg(lvl, fmt, ap);
    va_end(ap);

    switch (logger.type) {
        case LOG_T_STD:
            log_print_std(lvl, msg);
            break;
        case LOG_T_FILE:
            log_print_file(msg);
            break;
    }

    if (msg)
        free(msg);
}


void log_log_list(const char *const name, const t_node *head, void (*node_print)(const void *const, FILE *const)) {
    FILE *file = NULL;

    log_log(LOG_L_INFO, "Currently loaded %s ->\n", name);

    switch (logger.type) {
        case LOG_T_STD:
            file = stdout;
            break;
        case LOG_T_SYS:
            return;
        case LOG_T_FILE:
            file = logger.file;
            break;
        default:
            return;
    }

    list_print(head, file, node_print);
    fflush(file);
}


void log_exit(void) {
    log_log(LOG_L_INFO, "Shutting down");
    // Here, as logger, we cannot really do much with I/O errors
    if (logger.file)
        fclose(logger.file);
    if (logger.type == LOG_T_SYS)
        closelog();
}
