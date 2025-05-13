/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_double_comp
*/

#include "linked_dev.h"

bool list_double_eq(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(double *)a == *(double *)b;
}

bool list_double_gt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(double *)a > *(double *)b;
}

bool list_double_lt(void *a, void *b)
{
    if (!a || !b)
        return false;
    return *(double *)a < *(double *)b;
}
