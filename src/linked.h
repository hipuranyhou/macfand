/**
 * macfand - hipuranyhou - 18.01.2021
 * 
 * Daemon for controlling fans on Linux systems using
 * applesmc and coretemp.
 * 
 * https://github.com/Hipuranyhou/macfand
 */

#ifndef MACFAND_LINKED_H_retoeroioi
#define MACFAND_LINKED_H_retoeroioi

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Holds data used by generic single linked list.
 * This is used for constructing generic single linked list holding pointer to data and pointer to next node in list.
 */
typedef struct node {
    void *data;
    struct node *next; 
} t_node;

/**
 * @brief Prepends node to given generic linked list.
 * Prepends node to given generic linked list. Regardless of whether head is NULL or not, it will after
 * point to first node of the list. This list has to be freed afterwards with list_free().
 * @param[in,out]  head       Pointer to head of list.
 * @param[in]      data       Data to be saved in data member of node.
 * @param[in]      data_size  Size of data type saved in list.
 * @return int 0 on error, 1 on success.
 */
int list_push_front(t_node **head, const void *data, const size_t data_size);

/**
 * @brief Frees memory used by given generic linked list.
 * Frees memory used by the given list by calling the provided node_free() function on data member of every node
 * and free() on every node.
 * @param[in]  head       Pointer to head of generic linked list.
 * @param[in]  node_free  Pointer to free function for data type saved in generic linked list.
 */
void list_free(t_node *head, void (*node_free)(void *));

/**
 * @brief Prints given generic linked list.
 * Prints the given list by calling the provided node_print() function on data member of every node.
 * @param[in]  head        Pointer to head of generic linked list.
 * @param[in]  node_print  Pointer to print function for data type saved in generic linked list.
 */
void list_print(const t_node *head, FILE *stream, void (*node_print)(const void *, FILE *));

#endif //MACFAND_LINKED_H_retoeroioi