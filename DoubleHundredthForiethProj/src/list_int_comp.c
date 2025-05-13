/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_int_comp
*/

#include "linked_dev.h"

bool list_int_eq(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(int *)a == *(int *)b;
}

bool list_int_gt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(int *)a > *(int *)b;
}

bool list_int_lt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(int *)a < *(int *)b;
}
