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
 * @brief Creates string using given format and parameters.
 * Concatenates given paramteres using format with vsnprintf(). Allocates memory for given string which has to
 * be later freed.
 * @param[in]  format  Format used for concatenation.
 * @param[in]  ...     Parameters to concatenate. 
 * @return char* NULL on error, pointer to null terminated char* otherwise.
 */
char* concatenate_format(const char* format, ...);

/**
 * @brief Returns max of two given integers.
 * Returns max of two given integers.
 * @param[in]  a  Integer a. 
 * @param[in]  b  Integer b.
 * @return int The higher value of these two, b if they are the same.
 */
int max(const int a, const int b);

/**
 * @brief Returns min of two given integers.
 * Returns min of two given integers.
 * @param[in]  a  Integer a. 
 * @param[in]  b  Integer b.
 * @return int The lower value of these two, b if they are the same.
 */
int min(const int a, const int b);

#endif //MACFAND_HELPER_H_pifiohaods
