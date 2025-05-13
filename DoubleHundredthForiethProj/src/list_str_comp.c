/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_str_comp
*/

#include <string.h>
#include "linked_dev.h"

bool list_str_eq(void *a, void *b)
{
    if (!a || !b)
        return false;
    return strcmp((char *)a, (char *)b) == 0;
}
