/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MAC_FAN_CONTROL_HELPER_H_pifiohaods
#define MAC_FAN_CONTROL_HELPER_H_pifiohaods

/**
 *
 * @param format
 * @param ...
 * @return
 */
char* concatenate_format(const char* format, ...);

int max(int a, int b);
int min(int a, int b);

#endif //MAC_FAN_CONTROL_HELPER_H_pifiohaods
