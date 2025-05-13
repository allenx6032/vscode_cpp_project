/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_node_utils
*/

#include <stdlib.h>
#include "linked_dev.h"

void node_destroy(node_t *node, callback_t destroy)
{
    if (!node)
        return;
    if (node->data && destroy)
        destroy(node->data);
    free(node);
}

void node_destroy_all(node_t *node, callback_t destroy)
{
    node_t *prev = NULL;

    if (!node)
        return;
    while (node->prev)
        node = node->prev;
    while (node) {
        prev = node;
        node = node->next;
        node_isolate(prev);
        node_destroy(prev, destroy);
    }
}
