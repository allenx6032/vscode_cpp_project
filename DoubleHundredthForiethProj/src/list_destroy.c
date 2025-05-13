/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_destroy
*/

#include <stdlib.h>
#include "linked_dev.h"

void list_clear(list_t *list, callback_t destroy)
{
    if (!list)
        return;
    node_destroy_all(list->head, destroy);
    list->head = NULL;
    list->tail = NULL;
}

void list_destroy(list_t *list, callback_t destroy)
{
    if (!list)
        return;
    list_clear(list, destroy);
    free(list);
}

void list_clear_free(list_t *list)
{
    list_clear(list, free);
}

void list_destroy_free(list_t *list)
{
    list_destroy(list, free);
}
