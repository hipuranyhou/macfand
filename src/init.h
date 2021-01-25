/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_INIT_H_gfsdgsggsf
#define MACFAND_INIT_H_gfsdgsggsf

/**
 * @brief Prepares and start macfand.
 * Parses command line arguments using argp, registers signal handlers, loads and checks settings, 
 * sets fans to manual mode and starts main control loop. Sets fans back to automatic mode when exiting.
 * @param[in] argc Number of command line arguments.
 * @param[in] argv Passed command line arguments.
 * @return int 0 on error, 1 on success.
 */
int init_load(int argc, char **argv);

#endif //MACFAND_INIT_H_gfsdgsggsf