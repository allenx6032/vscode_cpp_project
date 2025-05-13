/*
** EPITECH PROJECT, 2024
** linked
** File description:
** conditional handling functions
*/

#include "linked_dev.h"

void *list_get_if(list_t *list, comparator_t compare, void *right)
{
    if (!list || !compare)
        return false;
    for (node_t *i = list->head; i; i = i->next) {
        if (compare(i->data, right))
            return i->data;
    }
    return NULL;
}

int list_move_if(list_t *from, list_t *to, comparator_t compare, void *right)
{
    size_t moved = 0;

    if (!from || !to || !compare || !right)
        return -1;
    for (size_t i = 0; i < from->size; i++) {
        if (compare(list_at(from, i), right)) {
            list_move(from, i, to);
            i--;
            moved++;
        }
    }
    return moved;
}
