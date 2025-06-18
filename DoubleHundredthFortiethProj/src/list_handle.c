/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_handle
*/

#include <stdlib.h>
#include "linked_dev.h"

static void *get_from_front(node_t *node, size_t count)
{
    while (count > 0 && node) {
        node = node->next;
        count--;
    }
    if (node)
        return node->data;
    return NULL;
}

static void *get_from_back(node_t *node, size_t count)
{
    while (count > 0 && node) {
        node = node->prev;
        count--;
    }
    if (node)
        return node->data;
    return NULL;
}

void *list_at(list_t *list, int index)
{
    if (list_empty(list))
        return NULL;
    if (index >= 0 && (size_t)index > list->size / 2)
        index = (list->size - index) * -1;
    if (index < 0)
        return get_from_back(list->tail, abs(index) - 1);
    return get_from_front(list->head,
        (size_t)index >= list->size ? list->size - 1 : (size_t)index);
}

bool list_move(list_t *from, int at, list_t *to)
{
    void *data = NULL;

    if (!from || !to)
        return false;
    data = list_at(from, at);
    if (!data)
        return false;
    if (!list_add_ptr(to, data))
        return false;
    return list_pop(from, at);
}

bool list_apply(list_t *list, applicator_t apply, void *accumulator)
{
    if (!list || !apply || !accumulator)
        return false;
    for (size_t i = 0; i < list->size; i++) {
        apply(accumulator, list_at(list, i));
    }
    return true;
}

bool list_map(list_t *list, callback_t map)
{
    if (!list || !map)
        return false;
    for (node_t *i = list->head; i; i = i->next) {
        map(i->data);
    }
    return true;
}

bool list_has_ptr(list_t *list, void *ptr)
{
    if (!list || !ptr)
        return false;
    for (node_t *i = list->head; i; i = i->next) {
        if (i->data == ptr)
            return true;
    }
    return false;
}
