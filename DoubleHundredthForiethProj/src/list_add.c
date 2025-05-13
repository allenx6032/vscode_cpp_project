/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_add
*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "linked_dev.h"

node_t *list_add_node(list_t *list, allocator_t allocate)
{
    node_t *node = NULL;

    if (!list)
        return NULL;
    node = node_create(allocate);
    if (!node)
        return NULL;
    node_link(list->tail, node, NULL);
    list->tail = node;
    if (!list->head)
        list->head = node;
    node->list = list;
    list_inc_size(list);
    return node;
}

void *list_add(list_t *list, allocator_t allocate)
{
    node_t *node = NULL;

    node = list_add_node(list, allocate);
    return node ? node->data : NULL;
}

void *list_add_ptr(list_t *list, void *ptr)
{
    node_t *node = NULL;

    node = list_add_node(list, NULL);
    if (node)
        node->data = ptr;
    return node ? node->data : NULL;
}

void *list_add_copy(list_t *list, void *data, size_t size)
{
    void *new = NULL;

    if (!list || !data || !size)
        return NULL;
    new = malloc(size);
    if (!new)
        return NULL;
    new = memcpy(new, data, size);
    return list_add_ptr(list, new);
}
