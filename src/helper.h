/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_HELPER_H_pifiohaods
#define MACFAND_HELPER_H_pifiohaods

/**
 * @brief 
 * 
 * @param format 
 * @param ... 
 * @return char* 
 */
char* concatenate_format(const char* format, ...);

/**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int max(int a, int b);

/**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int min(int a, int b);

#endif //MACFAND_HELPER_H_pifiohaods
