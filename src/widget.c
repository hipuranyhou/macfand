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
    FILE *widget_file = NULL;
    t_fan *fan = NULL;

    widget_file = fopen(set_get_val_str(SET_WIDGET_FILE_PATH), "w");
    if (!widget_file) {
        logger_log(LOG_L_ERROR, "%s", "Unable to open widget file");
        return;
    }

    while (fans) {
        fan = fans->data;
        if (fprintf(widget_file, "%d(f%d)%c", fan->spd.real, fan->id, (fans->next) ? ' ' : '\0') < 0)
            logger_log(LOG_L_ERROR, "%s %d %s", "Unable to write speed of fan", fan->id, "to widget file");
        fans = fans->next;
    }

    if (fclose(widget_file) == EOF) {
        logger_log(LOG_L_ERROR, "%s", "Unable to close widget file");
        return;
    }

}