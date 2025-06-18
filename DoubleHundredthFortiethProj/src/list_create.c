/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_create
*/

#include <stdlib.h>
#include "linked_dev.h"

node_t *node_create(allocator_t allocate)
{
    node_t *node = NULL;

    node = malloc(sizeof(node_t));
    if (!node)
        return NULL;
    node->data = allocate ? allocate() : NULL;
    if (allocate && !node->data) {
        free(node);
        return NULL;
    }
    node->list = NULL;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

list_t *list_create(void)
{
    list_t *list = NULL;

    list = malloc(sizeof(list_t));
    list_init(list);
    return list;
}

void list_init(list_t *list)
{
    if (!list)
        return;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
