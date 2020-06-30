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

#include <stdarg.h>

/**
 * @brief Concatenates values into a string
 * Concatenates values in ap into a string with given format. Allocates memory which has to be freed by caller.
 * @param[in]  format  Format of constructed string
 * @param[in]  ap      Values to be concatenated into a string
 * @return char* NULL on error, pointer to concatenated string otherwise (has to be freed!)
 */
char* concatenate_format_v(const char* format, va_list ap);

/**
 * @brief Wrapper for concatenate_format_v()
 * Wrapper for concatenate_format_v()
 * @param[in]  format  Format of constructed string
 * @param[in]  ...     Values to be concatenated into a string
 * @return char* NULL on error, pointer to concatenated string otherwise (has to be freed!)
 */
char* concatenate_format(const char* format, ...);

/**
 * @brief Validates and converts string to int.
 * Uses strtol() to check if given string is an valid integer and is in range of int. On success resulting
 * integer is stored in destination.
 * @param[in]  string       String which should be checked and converted to an int.
 * @param[out] destination  Pointer to where should the integer be stored.
 * @return int 0 on error, 1 if integer conversion succeded.
 */
int get_int_from_string(char *string, int *destination);

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
