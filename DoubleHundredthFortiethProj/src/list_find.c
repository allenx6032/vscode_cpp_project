/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_find
*/

#include "linked_dev.h"

int list_find(list_t *list, comparator_t compare, void *right)
{
    if (!list || !compare || !right)
        return -1;
    for (size_t i = 0; i < list->size; i++) {
        if (compare(list_at(list, i), right))
            return i;
    }
    return -1;
}

int list_find_ptr(list_t *list, void *ptr)
{
    if (!list || !ptr)
        return -1;
    for (size_t i = 0; i < list->size; i++) {
        if (list_at(list, i) == ptr)
            return i;
    }
    return -1;
}
