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

#include "linked.h"

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
 * to the chosen logger (std, file or syslog). When we are not in verbose mode, we
 * log only errors, nothing else.
 * @param[in]  level   Level of message priority (one of enum log_level).
 * @param[in]  format  Format of constructed message.
 * @param[in]  ...     Values to be concatenated into a message.
 */
void logger_log(int level, const char *format, ...);

/**
 * @brief Logs given generic linked list.
 * Logs given generic linked list using given print function based on logger type (file or std).
 * Printing of lists is disable when using syslog.
 * @param[in]  name        Name of logged list
 * @param[in]  head        Pointer to head of generic linked list.
 * @param[in]  node_print  Pointer to print function for data type saved in generic linked list.
 */
void logger_log_list(const char *name, const t_node *head, void (*node_print)(const void *, FILE *));

/**
 * @brief Logs exit message and gracefully exits logger
 * Logs exit message, and closes open log file and syslog if used.
 */
void logger_exit(void);

#endif //MACFAND_LOGGER_H_lfakfjakjl