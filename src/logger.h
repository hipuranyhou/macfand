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
 * @return char* 
 */
static int logger_get_time(int destination_size, char *destination);

/**
 * @brief 
 * 
 * @param type 
 * @return int 
 */ 
int logger_set_type(int type, char *file_path);

/**
 * @brief 
 * 
 */
void logger_exit(void);

/**
 * @brief 
 * 
 * @param level 
 * @param full_string 
 */
static void logger_print_std(int level, char *full_string);

/**
 * @brief 
 * 
 * @param full_string 
 */
static void logger_print_file(char *full_string);

/**
 * @brief 
 * 
 * @param level 
 * @param format 
 * @param ap 
 * @return char* 
 */
static char* logger_construct_full_string(int level, const char *format, va_list ap);

/**
 * @brief 
 * 
 * @param level 
 * @param format 
 * @param ... 
 */
void logger_log(int level, const char *format, ...);

#endif //MACFAND_LOGGER_H_lfakfjakjl