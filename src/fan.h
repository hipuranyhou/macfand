/**
 * macfand - hipuranyhou - 22.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_FAN_H_qwewqiorhq
#define MACFAND_FAN_H_qwewqiorhq

#include <stdio.h>

#include "linked.h"

/**
 * @brief Fan speeds struct.
 * Struct holding all speeds of fan, which are min, max, real (current)
 * target when changing speed and step (all in RPM, dictated by applesmc).
 */
struct fan_spd {
    int min;
    int max;
    int real;
    int tgt;
    int step;
};

/**
 * @brief Fan paths struct.
 * Struct holding all used paths of fan, which are rd for reading,
 * wr for writing and mod for setting mode (auto/manual).
 */
struct fan_path {
    char *rd;
    char *wr;
    char *mod;
};

/**
 * @brief Fan type.
 * Type for system fan holding id, label, speeds and paths.
 */
typedef struct fan {
    int             id;
    char            *lbl;
    struct fan_spd  spd;
    struct fan_path path;
} t_fan;

/**
 * @brief Enum holding fan modes.
 * Enum holding fan modes (auto/manual) allowed by the applesmc module.
 */
enum fan_mode {
    FAN_M_AUTO,
    FAN_M_MAN
};

/**
 * @brief Constructs linked list of system fans.
 * Constructs generic linked list of unlimited number of system fans. For each fan sets its id, real and target
 * speed to 0, from appropriate system files loads its label, min and max speed. Based on these values 
 * calculates step size of speed adjust for given fan and constructs its read, write and mode setting paths.
 * @return t_node* NULL on error, pointer to head of generic linked list of system fans otherwise.
 */
t_node *fans_load(void);

/**
 * @brief Sets mode of all system fans.
 * Sets operating mode (automatic or manual) of all system fans by writing to the appropriate system files.
 * @param[in,out] fans Pointer to head of generic linked list of system fans.
 * @param[in]     mod  Mode to which should fans be set (one of FAN_M_AUTO and FAN_M_MAN). 
 * @return int 0 if at least one setting of fan failed, 1 on success.
 */
int fans_write_mod(const t_node *fans, const enum fan_mode mod);

/**
 * @brief Sets speed of given fan.
 * Read current real speed of given fan and sets new speed by writing to the appropriate system file.
 * @param[in,out] fan Pointer to fan.
 * @return int 0 on error, 1 on success.
 */
int fan_write_spd(t_fan *const fan);

/**
 * @brief Frees memory for given fan.
 * Calls free() on all allocated members of given fan if they are not NULL and fan itself.
 * @param[in]  fan  Pointer to fan.
 */
void fan_free(t_fan *fan);

/**
 * @brief Prints info about fan.
 * Prints formatted information (all members of t_fan) about given fan to stdout.
 * @param[in]  fan  Pointer to fan. 
 */
void fan_print(const t_fan *const fan, FILE *const file);

#endif //MACFAND_FAN_H_qwewqiorhq
