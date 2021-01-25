/**
 * macfand - hipuranyhou - 20.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_CONFIG_H_nuviuvnsiv
#define MACFAND_CONFIG_H_nuviuvnsiv

/**
 * @brief Parses configuration file at given location.
 * Parses configuration file at given location and based on this updates 
 * appropriate settings. Does not check validity of settings.
 * @param[in] path Path to macfand configuration file.
 * @return int 0 on error, 1 on success.
 */
int conf_load(const char *path);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf