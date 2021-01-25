/**
 * macfand - hipuranyhou - 25.01.2021
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
 * @param[in] fmt Format of constructed string.
 * @param[in] ap  Values to be concatenated into a string.
 * @return char* NULL on error, pointer to concatenated string otherwise (has to be freed!)
 */
char* v_concat_fmt(const char *const fmt, va_list ap);

/**
 * @brief Wrapper for v_concat_fmt(). Concatenates values into a string.
 * Wrapper for v_concat_fmt(). Concatenates values into a string.
 * @param[in] fmt Format of constructed string.
 * @param[in] ... Values to be concatenated into a string.
 * @return char* NULL on error, pointer to concatenated string otherwise (has to be freed!)
 */
char* concat_fmt(const char *const fmt, ...);

/**
 * @brief Extracts word until delimeter (not including), newline or null byte.
 * Extracts word until delimeter, newline or null byte, whichever comes first. Also skips whitespace
 * at the beginning of given string. If *dest is NULL and *dest_size is 0, allocates buffer using malloc.
 * If *dest is not null and *dest_size is not zero, reallocates given buffer. Maximum size of buffer
 * which will be allocated is 4096 bytes.
 * @param[in]     str       String from which we extract word.
 * @param[in]     delim     Delimeter until which we read.
 * @param[out]    dest      Address of destination buffer.
 * @param[in,out] dest_size Address of destination buffer size. 
 * @return ssize_t -1 on error, number of extracted characters not including terminating null byte otherwise.
 */
ssize_t get_str_until(const char *str, const char delim, char **dest, size_t *const dest_size);

/**
 * @brief Converts string to integer
 * Converts given to string to an integer using strtol(). If inv is not NULL, sets *inv
 * to first invalid character in string. Final integer is saved in *dest.
 * @param[in]  str  String from which we extract number.
 * @param[out] dest Address of destination.
 * @param[in]  base Base in which read number is (2-36 including both).
 * @param[out] inv  Address where should be first invalid character saved.
 * @return int -1 on error, 0 on partly converted string, 1 on success
 */
int str_to_int(const char *const str, int *const dest, int base, char *const inv);

/**
 * @brief Returns max of two given integers.
 * Returns max of two given integers.
 * @param[in] a Integer a. 
 * @param[in] b Integer b.
 * @return int The higher value of these two, b if they are the same.
 */
int max(const int a, const int b);

/**
 * @brief Returns min of two given integers.
 * Returns min of two given integers.
 * @param[in] a Integer a. 
 * @param[in] b Integer b.
 * @return int The lower value of these two, b if they are the same.
 */
int min(const int a, const int b);

#endif //MACFAND_HELPER_H_pifiohaods
