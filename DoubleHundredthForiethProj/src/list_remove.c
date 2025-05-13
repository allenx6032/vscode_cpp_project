/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_remove
*/

#include <stdlib.h>
#include "linked_dev.h"

static void pop_from_front(list_t *list, size_t count, void **data)
{
    node_t *node = list ? list->head : NULL;
    node_t *prev = node ? node->prev : NULL;
    node_t *next = node ? node->next : NULL;

    while (count > 0 && node) {
        prev = node;
        node = node->next;
        next = node ? node->next : NULL;
        count--;
    }
    node_isolate(node);
    if (node) {
        *data = node->data;
        node_destroy(node, NULL);
        if (!prev)
            list_set_head(list, next);
        if (!next)
            list_set_tail(list, prev);
    }
}

static void pop_from_back(list_t *list, size_t count, void **data)
{
    node_t *node = list ? list->tail : NULL;
    node_t *prev = node ? node->prev : NULL;
    node_t *next = node ? node->next : NULL;

    while (count > 0 && node) {
        next = node;
        node = node->prev;
        prev = node ? node->prev : NULL;
        count--;
    }
    node_isolate(node);
    if (node) {
        *data = node->data;
        node_destroy(node, NULL);
        if (!prev)
            list_set_head(list, next);
        if (!next)
            list_set_tail(list, prev);
    }
}

void *list_pop(list_t *list, int index)
{
    node_t *node = NULL;
    void *data = NULL;

    if (list_empty(list))
        return NULL;
    if (list->size == 1) {
        node = list->head;
        data = node->data;
        list->head = NULL;
        list->tail = NULL;
        node_destroy(node, NULL);
        return data;
    }
    if (index < 0)
        pop_from_back(list, abs(index) - 1, &data);
    else
        pop_from_front(list, (size_t)index >= list->size
            ? list->size - 1 : (size_t)index, &data);
    return data;
}

bool list_remove(list_t *list, callback_t destroy, int index)
{
    node_t *node = NULL;

    node = list_pop(list, index);
    if (node && destroy) {
        destroy(node);
        return true;
    }
    return false;
}

bool list_remove_free(list_t *list, int index)
{
    return list_remove(list, free, index);
}

int list_remove_if(list_t *list, comparator_t compare, void *right,
    callback_t destroy)
{
    int removed = 0;

    if (!list || !compare)
        return -1;
    for (size_t i = 0; i < list->size; i++) {
        if (compare(list_at(list, i), right)) {
            list_remove(list, destroy, i);
            i--;
            removed++;
        }
    }
    return removed;
}

int list_remove_if_free(list_t *list, comparator_t compare, void *right)
{
    return list_remove_if(list, compare, right, free);
}
