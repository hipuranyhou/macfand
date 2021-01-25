/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdio.h>

#include "widget.h"
#include "settings.h"
#include "logger.h"
#include "fan.h"


void wgt_write(const t_node *fans) {
    FILE  *file = NULL;
    t_fan *fan  = NULL;

    file = fopen(set_get_val_str(SET_WIDGET_FILE_PATH), "w");
    if (!file) {
        log_log(LOG_L_ERROR, "%s", "Unable to open widget file");
        return;
    }

    while (fans) {
        fan = fans->data;
        if (fprintf(file, "%d(f%d)%c", fan->spd.real, fan->id, (fans->next) ? ' ' : '\0') < 0)
            log_log(LOG_L_ERROR, "%s %d %s", "Unable to write speed of fan", fan->id, "to widget file");
        fans = fans->next;
    }

    if (fclose(file) == EOF)
        log_log(LOG_L_ERROR, "%s", "Unable to close widget file");
}