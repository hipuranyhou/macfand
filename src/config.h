/**
 * macfand - hipuranyhou - 21.06.2020
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
 * @param[in]  config_path  Path to macfand configuration file.
 * @return int 0 on error, 1 on success.
 */
int config_load(const char *config_path);

#endif //MACFAND_CONTROL_H_fsdfdsfsdf