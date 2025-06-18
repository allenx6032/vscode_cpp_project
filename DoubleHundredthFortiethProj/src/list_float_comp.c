/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_float_comp
*/

#include "linked_dev.h"

bool list_float_eq(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(float *)a == *(float *)b;
}

bool list_float_gt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(float *)a > *(float *)b;
}

bool list_float_lt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(float *)a < *(float *)b;
}
