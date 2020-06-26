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
    LOG_L_DEBUG
};

/**
 * @brief 
 * 
 * @param type 
 */
void logger_set_mode(const enum log_type type);

#endif //MACFAND_LOGGER_H_lfakfjakjl