/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_FAN_H_qwewqiorhq
#define MACFAND_FAN_H_qwewqiorhq

#include "settings.h"
#include "linked.h"

/**
 * @brief Holds information about system fan.
 * Struct holding id, min and max speed, current speed, speed change step size, paths for writing and
 * setting mode and label of a system fan.
 */
typedef struct fan {
    int id;
    int min;
    int max;
    int speed;
    int step;
    char *path_write;
    char *path_manual;
    char *label;
} t_fan;

/**
 * @brief Enum holding fan modes.
 * Enum holding fan modes allowed by the applesmc module.
 */
enum fan_mode {
    FAN_AUTO,
    FAN_MANUAL
};

/**
 * @brief Loads label of fan.
 * Loads label of given fan by reading the appropriate system file.
 * @param[in,out]  fan  Pointer to fan. 
 * @return int 0 on error, 1 on success.
 */
static int fan_load_label(t_fan *fan);

/**
 * @brief Loads given speed of fan.
 * Loads given speed to destination of fan by reading the appropriate system file.
 * @param[in,out]  fan          Pointer to fan. 
 * @param[out]     destination  Pointer to where should be the speed saved.
 * @param[in]      speed        Path end of wanted speed (one of FAN_PATH_MAX and FAN_PATH_MIN).
 * @return int 0 on error, 1 on success.
 */ 
static int fan_load_speed(t_fan *fan, int *destination, const char *speed);

/**
 * @brief Loads default values for given fan.
 * Loads max and min speed of given fan, calculates its step size based on these values, constructs its
 * writing and mode settings paths and finally loads its label.
 * @param[in]     settings  Pointer to struct holding macfand settings.
 * @param[in,out] fans      Pointer to head of linked list of system fans.
 * @return int 0 on error, 1 on success.
 */
static int fan_load_defaults(const t_settings *settings, t_fan *fan);

/**
 * @brief Check if fan exists.
 * Checks whether fan with given id exists by trying to opening its mode setting path.
 * @param[in]  id_fan  id of wanted fan.
 * @return int -1 on error, 0 if does not exist, 1 if exists.
 */
static int fan_id_exists(const int id_fan);

/**
 * @brief Constructs linked list of system fans.
 * Constructs generic linked list of unlimited number of system fans. For each fan sets its id, current speed to 0,
 * from appropriate system files loads its label, min and max speed. Based on these values calculates step size
 * of speed adjust for given fan and constructs its write and mode setting path.
 * @param[in]  settings  Pointer to struct holding macfand settings.
 * @return t_node* NULL on error, pointer to head of generic linked list of system fans otherwise.
 */
t_node *fans_load(const t_settings *settings);

/**
 * @brief Sets mode of all system fans.
 * Sets operating mode (automatic or manual) of all system fans by writing to the appropriate system files.
 * @param[in,out]  fans  Pointer to head of linked list of system fans.
 * @param[in]      mode  Mode to which should fans be set. 
 * @return int 0 if at least one setting of fan failed, 1 on success.
 */
int fans_set_mode(t_node *fans, int mode);

/**
 * @brief Sets speed of given fan.
 * Sets new speed of given fan using by writing to the appropriate system file.
 * @param[in,out]  fan    Pointer to fan.
 * @param[in]      speed  Speed to which should fan be set. 
 * @return int 0 on error, 1 on success.
 */
int fan_set_speed(t_fan *fan, const int speed);

/**
 * @brief Frees memory for given fan.
 * Calls free() on all allocated members of given fan if they are not NULL.
 * @param[in]  fan  Pointer to fan.
 */
void fan_free(t_fan *fan);

/**
 * @brief Prints info about fan.
 * Prints formatted information (all members of t_fan) about given fan to stdout.
 * @param[in]  fan  Pointer to fan. 
 */
void fan_print(const t_fan *fan);

#endif //MACFAND_FAN_H_qwewqiorhq
