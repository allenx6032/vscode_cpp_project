/*
** EPITECH PROJECT, 2021
** vector
** File description:
** Vector List (like cpp but in c)
*/

/*
    Buffer * 2 when too much data
    Resized when div 2 possible
*/

#include <stdlib.h>
#include "engine_vector.h"

void my_vector_init(vector_t *vec)
{
    vec->buffer_size = 1;
    vec->size = 0;
    vec->items = malloc(sizeof(void *) * vec->buffer_size);
}

void my_vector_resize(vector_t *vec, int size)
{
    void **items = malloc(sizeof(void *) * size);

    for (int i = 0; i != vec->size; i++)
        items[i] = vec->items[i];
    free(vec->items);
    vec->items = items;
    if (items) {
        vec->buffer_size = size;
        vec->items = items;
    }
}

void my_vector_add(vector_t *vec, void *item)
{
    if (vec->buffer_size == vec->size)
        my_vector_resize(vec, vec->buffer_size * 2);
    vec->items[vec->size++] = item;
}

void *my_vector_get(vector_t *vec, int index)
{
    if (index >= 0 && index < vec->size)
        return vec->items[index];
    return NULL;
}

void my_vector_delete(vector_t *vec, int index)
{
    if (index < 0 || index >= vec->size)
        return;
    vec->items[index] = NULL;
    for (int i = index; i < vec->size - 1; i++) {
        vec->items[i] = vec->items[i + 1];
        vec->items[i + 1] = NULL;
    }
    vec->size--;
    if (vec->size > 0 && vec->size == vec->buffer_size / 2)
        my_vector_resize(vec, vec->buffer_size / 2);
}