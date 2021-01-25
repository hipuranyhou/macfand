/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include "stdlib.h"

#include "init.h"

int main(int argc, char **argv) {
    if (!init_load(argc, argv))
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
