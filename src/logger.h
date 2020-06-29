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
    LOG_L_DEBUG,
    LOG_L_UNKNOWN
};

/**
 * @brief 
 * 
 * @return char* 
 */
static int logger_get_time(int destination_size, char *destination);

/**
 * @brief 
 * 
 * @param level 
 * @param format 
 * @param ... 
 */
void logger_log(int level, const char *format, ...);

#endif //MACFAND_LOGGER_H_lfakfjakjl