/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_LOGGER_H_lfakfjakjl
#define MACFAND_LOGGER_H_lfakfjakjl

#include <stdarg.h>

/**
 * @brief 
 * 
 */
enum log_type {
    LOG_T_STD,
    LOG_T_SYS,
    LOG_T_FILE
};

/**
 * @brief 
 * 
 */
enum log_level {
    LOG_L_ERROR,
    LOG_L_WARN,
    LOG_L_INFO,
    LOG_L_DEBUG
};

/**
 * @brief Constructs time string for logging.
 * Constructs time string with format given in LOGGER_DATE_FORMAT into destination.
 * @param[in]   destination_size  Size of buffer for time string.
 * @param[out]  destination       Pointer to destination of time string.
 * @return int 0 on error, 1 on success.
 */
static int logger_get_time(int destination_size, char *destination);

/**
 * @brief Prints given string to std.
 * Prints given string to stderr for level LOG_L_ERROR and to stdout for other levels.
 * @param[in]  level        Level of message priority (one of enum log_level).
 * @param[in]  full_string  String to be logged.
 */
static void logger_print_std(int level, char *full_string);

/**
 * @brief Prints given string to log file.
 * Prints given string to log file.
 * @param[in]  full_string  String to be logged.
 */
static void logger_print_file(char *full_string);

/**
 * @brief Construct full logged string.
 * Constructs full logged string by getting current time, level string and message with given format
 * and concatenates these together. Allocates memory which has to be freed by caller.
 * @param[in]  level   Level of message priority (one of enum log_level).
 * @param[in]  format  Format of constructed message.
 * @param[in]  ap      Values to be concatenated into a string.
 * @return char* NULL on error, pointer to full log string otherwise (has to be free by caller).
 */
static char* logger_construct_full_string(int level, const char *format, va_list ap);

/**
 * @brief Sets logger to given mode.
 * Sets logger to given mode. Before this, closes previous log (file or syslog). For type LOG_T_FILE 
 * opens given log file for appending, for type LOG_T_SYS open syslog.
 * @param[in]  type       Type of logger to be used (one of enum log_type).
 * @param[in]  file_path  Path to log file if LOG_T_FILE is used (NULL otherwise).
 * @return int 0 on error, 1 on success.
 */ 
int logger_set_type(int type, char *file_path);

/**
 * @brief Logs event.
 * Constructs full logged string including time and level string and passes it 
 * to the chosen logger (std, file or syslog).
 * @param[in]  level   Level of message priority (one of enum log_level).
 * @param[in]  format  Format of constructed message.
 * @param[in]  ...     Values to be concatenated into a message.
 */
void logger_log(int level, const char *format, ...);

/**
 * @brief Logs exit message and gracefully exits logger
 * Logs exit message, and closes open log file and syslog if used.
 */
void logger_exit(void);

#endif //MACFAND_LOGGER_H_lfakfjakjl