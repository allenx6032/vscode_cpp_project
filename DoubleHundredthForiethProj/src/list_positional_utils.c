/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_positional_utils
*/

#include "linked_dev.h"

void list_set_head(list_t *list, node_t *node)
{
    if (!list || !node)
        return;
    if (list->head && list->head != node) {
        list->head->prev = node;
        node->next = list->head;
    }
    list->head = node;
}

void list_set_tail(list_t *list, node_t *node)
{
    if (!list || !node)
        return;
    if (list->tail && list->tail != node) {
        list->tail->next = node;
        node->prev = list->tail;
    }
    list->tail = node;
}
