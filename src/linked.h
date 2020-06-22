/**
 * macfand - hipuranyhou - 21.06.2020
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_LINKED_H_retoeroioi
#define MACFAND_LINKED_H_retoeroioi

#include <stdlib.h>

/**
 * 
 */
typedef struct node {
    void *data;
    struct node *next; 
} t_node;

/**
 * 
 */
int list_push_front(t_node **head, const void *data, size_t data_size);

/**
 * 
 */
void list_free(t_node *head, void (*free_node)(void *));

/**
 * 
 */
void list_print(const t_node *head);

#endif //MACFAND_LINKED_H_retoeroioi