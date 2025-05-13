/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_char_comp
*/

#include "linked_dev.h"

bool list_char_eq(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(char *)a == *(char *)b;
}

bool list_char_gt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(char *)a > *(char *)b;
}

bool list_char_lt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(char *)a < *(char *)b;
}
