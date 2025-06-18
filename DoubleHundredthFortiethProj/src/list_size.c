/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_size
*/

#include "linked_dev.h"

bool list_empty(const list_t *list)
{
    return !list || list->size == 0;
}

void list_inc_size(list_t *list)
{
    if (!list)
        return;
    list->size++;
}

void list_dec_size(list_t *list)
{
    if (!list || list->size == 0)
        return;
    list->size--;
}
