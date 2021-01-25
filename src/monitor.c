/**
 * macfand - hipuranyhou - 25.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#include "monitor.h"
#include "helper.h"
#include "settings.h"
#include "logger.h"

#define MON_PATH_CLS  "/sys/class/hwmon"
#define MON_PATH_BASE "/sys/devices/platform/coretemp.0/hwmon"
#define MON_PATH_RD   "input"
#define MON_PATH_MAX  "max"
#define MON_PATH_LBL  "label"
#define MON_PATH_FMT  MON_PATH_BASE "/hwmon%d/temp%d_%s"

/**
 * @brief Loads label of given temperature monitor.
 * Loads label of given temperature monitor into mon->lbl by reading appropriate system file.
 * @param[in,out] mon Monitor to be updated.
 * @return int 0 on error, 1 on success.
 */
static int mon_load_lbl(t_mon *const mon);

/**
 * @brief Selects where should monitor read temperature end and from where should be read.
 * Changes *dest to proper location of where should be temperature saved after reading and *path
 * to from where should temperature for given monitors be read based on path suffix.
 * @param[in]  mon  Monitor which temperature we read.
 * @param[in]  suff Suffix of temperature file from which we read temperature.
 * @param[out] dest Pointer where should be read value destination saved.
 * @param[out] path Pointer where should be read path saved.
 * @return int 0 on error, 1 on success
 */
static int mon_sel_temp_dest(t_mon *const mon, const char *const suff, int **const dest, char **const path);

/**
 * @brief Loads temperature of given monitor.
 * If suff is NULL, reads current temperature of monitor into mon->temp.real. If suff is 
 * MON_PATH_MAX, reads max temperature of given monitor into mon->temp.max.
 * @param[in,out] mon  Monitor to be updated.
 * @param[in]     suff Suffix of temperature file from which we read temperature.
 * @return int 0 on error, 1 on success.
 */
static int mon_read_temp(t_mon *const mon, const char *const suff);

/**
 * @brief Loads defaults of given monitor.
 * Loads label, read path, max temperature and sets real temperature to 0.
 * @param[in,out] mon Monitor to be updated.
 * @return int 0 on error, 1 on success.
 */
static int mon_load_def(t_mon *const mon);

/**
 * @brief Finds hwmon id of coretemp.0.
 * Finds hwmon id of coretemp.0 using symlink in /sys/class/hwmon.
 * @return int -1 on error, coretemp.0 hwmon entry id otherwise.
 */
static int mons_find_hw_id(void);


static int mon_load_lbl(t_mon *const mon) {
    char    *path    = NULL;
    FILE    *file    = NULL;
    ssize_t get_ret  = 0;
    size_t  lbl_size = 0;

    if (!mon)
        return 0;

    mon->lbl = NULL;

    path = concat_fmt(MON_PATH_FMT, mon->id.hw, mon->id.mon, MON_PATH_LBL);
    if (!path)
        return 0;

    file = fopen(path, "r");
    free(path);
    if (!file) {
        log_log(LOG_L_DEBUG, "Unable to open label file of monitor %d", mon->id.mon);
        return 0;
    }

    errno = 0;
    get_ret = getline(&(mon->lbl), &lbl_size, file);
    if (get_ret < 2 || errno != 0) {
        log_log(LOG_L_DEBUG, "Unable to load label of monitor %d", mon->id.mon);
        if (fclose(file) == EOF)
            log_log(LOG_L_DEBUG, "Unable to close label file of monitor %d", mon->id.mon);
        return 0;
    }

    // (get_ret - 1) because line ends with "0x0A"
    mon->lbl[get_ret-1] = '\0';

    if (fclose(file) == EOF)
        log_log(LOG_L_DEBUG, "Unable to close label file of monitpr %d", mon->id.mon);
    return 1;
}


static int mon_sel_temp_dest(t_mon *const mon, const char *const suff, int **const dest, char **const path) {
    if (!mon || !suff || !dest || !path)
        return 0;

    if (strcmp(MON_PATH_MAX, suff) == 0) {
        *dest = &(mon->temp.max);
        *path = concat_fmt(MON_PATH_FMT, mon->id.hw, mon->id.mon, MON_PATH_MAX);
    } else
        return 0;

    if (!(*path))
        return 0;
    return 1;
}


static int mon_read_temp(t_mon *const mon, const char *const suff) {
    char    *str     = NULL;
    size_t  str_size = 0;
    ssize_t get_ret  = 0;
    FILE    *file    = NULL;
    int     *rd_dest = &(mon->temp.real);
    char    *rd_path = mon->path_rd;

    if (!mon || (suff && !mon_sel_temp_dest(mon, suff, &rd_dest, &rd_path)))
        return 0;

    // Open file for reading
    file = fopen(rd_path, "r");
    if (!file) {
        log_log(LOG_L_DEBUG, "Unable to open temperature file of monitor %d", mon->id.mon);
        return 0;
    }

    // Read line from speed file and remove trailing '\n'
    errno = 0;
    get_ret = getline(&str, &str_size, file);
    if (get_ret < 2 || errno != 0) {
        log_log(LOG_L_DEBUG, "Invalid temperature file of monitor %d", mon->id.mon);
        if (fclose(file) == EOF)
            log_log(LOG_L_DEBUG, "Unable to close temperature file of monitor %d", mon->id.mon);
        return 0;
    }
    str[get_ret-1] = '\0';

    // Convert read line to integer
    if (str_to_int(str, rd_dest, 10, NULL) < 1) {
        log_log(LOG_L_DEBUG, "Invalid temperature of monitor %d", mon->id.mon);
        if (fclose(file) == EOF)
            log_log(LOG_L_DEBUG, "Unable to close temperature file of monitor %d", mon->id.mon);
        return 0;
    }

    if (fclose(file) == EOF)
        log_log(LOG_L_DEBUG, "Unable to close temperature file of monitor %d", mon->id.mon);
    return 1;
}


static int mon_load_def(t_mon *const mon) {
    if (!mon)
        return 0;

    mon->path_rd = concat_fmt(MON_PATH_FMT, mon->id.hw, mon->id.mon, MON_PATH_RD);
    if (!mon->path_rd)
        return 0;

    if (!mon_read_temp(mon, MON_PATH_MAX)) {
        log_log(LOG_L_DEBUG, "Unable to load max temperature of monitor %d", mon->id.mon);
        return 0;
    }

    mon->temp.real = 0;

    if (!mon_load_lbl(mon)) {
        log_log(LOG_L_DEBUG, "Unable to load label of monitor %d", mon->id.mon);
        return 0;
    }

    return 1;
}


static int mons_find_hw_id(void) {
    int           id         = -1;
    struct dirent *dirent    = NULL;
    DIR           *dir       = NULL;
    char          *fname     = NULL;
    char          *lpath     = NULL;
    ssize_t       llen       = 0;
    const size_t  ldest_size = 256;
    char          ldest[ldest_size];

    dir = opendir(MON_PATH_BASE);
    if (!dir)
        return -1;

    while ((dirent = readdir(dir))) {
        fname = basename(dirent->d_name);

        if (dirent->d_type != DT_LNK || !fname || strncmp(fname, "hwmon", 5) != 0)
            continue;

        lpath = concat_fmt("%s/%s", MON_PATH_CLS, fname);
        if (!lpath)
            return -1;

        errno = 0;
        llen = readlink(lpath, ldest, ldest_size-1);
        if (llen < 1 || errno != 0)
            return -1;
        ldest[llen] = '\0';

        if (!strstr(ldest, "coretemp.0"))
            continue;

        if (str_to_int(fname+5, &id, 10, NULL) < 1)
            return -1;
        
        break;
    }

    return id;
}


t_node* mons_load(void) {
    struct dirent *dirent    = NULL;
    DIR           *dir       = NULL;
    char          *hw_path   = NULL;
    char          *fname     = NULL;
    char          inv        = 0;
    int           id_prev    = 0;
    int           to_int_ret = 0;
    t_mon         mon;
    t_node        *mons      = NULL;

    mon.id.hw = mons_find_hw_id();
    if (mon.id.hw < 0) {
        log_log(LOG_L_DEBUG, "Unable to locate coretemp hwmon entry.");
        return NULL;
    }

    hw_path = concat_fmt("%s/hwmon%d", MON_PATH_BASE, mon.id.hw);
    if (!hw_path)
        return NULL;

    // Open fans directory
    dir = opendir(hw_path);
    if (!dir) {
        log_log(LOG_L_DEBUG, "Unable to open system monitors directory");
        return NULL;
    }

    // Walk through fans directory
    while ((dirent = readdir(dir))) {
        fname = basename(dirent->d_name);

        if (!fname || strncmp(fname, "temp", 4) != 0)
            continue;

        // Get id of fan
        to_int_ret = str_to_int(fname+4, &(mon.id.mon), 10, &inv);
        if (to_int_ret < 0 || inv != '_') {
            list_free(mons, (void (*)(void *, int))mon_free);
            mon_free(&mon, 0);
            log_log(LOG_L_DEBUG, "Invalid monitor filename encountered.");
            return NULL;
        }

        // Skip already finished fan
        if (mon.id.mon == id_prev)
            continue;
        id_prev = mon.id.mon;

        // Load fan defaults and append it to linked list of fans
        if (!mon_load_def(&mon) || !list_push_front(&mons, &mon, sizeof(mon))) {
            list_free(mons, (void (*)(void *, int))mon_free);
            mon_free(&mon, 0);
            log_log(LOG_L_DEBUG, "Unable to load defaults of monitor %d", mon.id.mon);
            return NULL;
        }
    }

    if (closedir(dir) < 0)
        log_log(LOG_L_DEBUG, "Unable to close system monitors directory");
    return mons;
}


int mons_get_temp(t_node *mons) {
    int   temp  = -1;
    t_mon *mon  = NULL;

    while (mons) {
        mon = mons->data;

        if (!mon_read_temp(mon, NULL)) {
            log_log(LOG_L_DEBUG, "Unable to read temperature from monitor %d", mon->id.mon);
            mons = mons->next;
            continue;
        }

        if (temp < mon->temp.real)
            temp = mon->temp.real;
        
        mons = mons->next;
    }

    // If failed to load at least one temperature, crank up the fans
    if (temp < 0) {
        log_log(LOG_L_ERROR, "Unable to read temperature from monitors.");
        return set_get_val(SET_TEMP_HIGH);
    }

    return (temp / 1000);
}


int mons_get_temp_max(const t_node *mons) {
    int   temp = -1;
    t_mon *mon = NULL;

    while (mons) {
        mon = mons->data;

        if (temp > mon->temp.max)
            temp = mon->temp.max;

        mons = mons->next;
    }

    if (temp < 0) {
        log_log(LOG_L_ERROR, "Unable to read max temperature from monitors.");
        return -1;
    }

    return (temp / 1000);
}


void mon_free(t_mon *mon, int self) {
    if (!mon)
        return;

    if (mon->path_rd)
        free(mon->path_rd);
    if (mon->lbl)
        free(mon->lbl);
    
    if (self)
        free(mon);
}


void mon_print(const t_mon *const mon, FILE *const file) {
    if (!mon)
        return;

    fprintf(file, "Monitor %d - %s\n", mon->id.mon, mon->lbl);
    fprintf(file, "Max temp: %d°C\n", mon->temp.max);
    fprintf(file, "Read: %s\n", mon->path_rd);
}