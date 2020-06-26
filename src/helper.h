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
 * @brief Validates and converts string to int.
 * Uses strtol() to check if given string is an valid integer and is in range of int. On success resulting
 * integer is stored in destination.
 * @param[in]  string       String which should be checked and converted to an int.
 * @param[out] destination  Pointer to where should the integer be stored.
 * @return int 0 on error, 1 if integer conversion succeded.
 */
int convert_valid_int(char *string, int *destination);

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
