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
 * @brief 
 * 
 */
typedef struct node {
    void *data;
    struct node *next; 
} t_node;

/**
 * @brief 
 * 
 * @param head 
 * @param data 
 * @param data_size 
 * @return int 
 */
int list_push_front(t_node **head, const void *data, const size_t data_size);

/**
 * @brief 
 * 
 * @param head 
 * @param node_free
 */
void list_free(t_node *head, void (*node_free)(void *));

/**
 * @brief 
 * 
 * @param head 
 * @param node_print 
 */
void list_print(const t_node *head, void (*node_print)(void *));

#endif //MACFAND_LINKED_H_retoeroioi