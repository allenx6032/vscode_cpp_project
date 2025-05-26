/*
** EPITECH PROJECT, 2021
** my_vector_more
** File description:
** my_vector_more
*/

#include <stdlib.h>
#include "engine_vector.h"

int my_vector_size(vector_t *vec)
{
    return vec->size;
}

void my_vector_set(vector_t *vec, void *item, int index)
{
    if (index < vec->size && index >= 0)
        vec->items[index] = item;
}

void my_vector_free(vector_t *vec)
{
    free(vec->items);
}