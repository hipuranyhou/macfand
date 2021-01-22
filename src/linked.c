/**
 * macfand - hipuranyhou - 22.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#include <string.h>

#include "linked.h"


int list_push_front(t_node **head, const void *const data, const size_t data_size) {
    t_node *node = (t_node*)malloc(sizeof(*node));

    if (!node || !data || data_size == 0)
        return 0;
    
    node->next = *head;
    node->data = malloc(data_size);

    if (!node->data) {
        free(node);
        return 0;
    }

    memcpy(node->data, data, data_size);
    *head = node;
    return 1;
}


void list_free(t_node *head, void (*node_free)(void *)) {
    t_node *next = NULL;
    while (head) {
        next = head->next;
        node_free(head->data);
        free(head);
        head = next;
    }
}


void list_print(const t_node *head, FILE *const file, void (*node_print)(const void *const, FILE *const)) {
    while (head) {
        node_print(head->data, file);
        fprintf(file, "\n");
        head = head->next;
    }
}