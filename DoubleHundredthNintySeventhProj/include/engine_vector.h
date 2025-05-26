/*
** EPITECH PROJECT, 2021
** vector
** File description:
** Cut from V.M. Lib for optimized include
*/

#ifndef VECTOR_H_
#define VECTOR_H_

typedef struct vector_s {
    void **items;
    int buffer_size;
    int size;
} vector_t;

void my_vector_init(vector_t *vec);
void my_vector_resize(vector_t *vec, int size);
void my_vector_add(vector_t *vec, void *item);
void *my_vector_get(vector_t *vec, int index);
void my_vector_delete(vector_t *vec, int index);
int my_vector_size(vector_t *vec);
void my_vector_set(vector_t *vec, void *item, int index);
void my_vector_free(vector_t *vec);

#endif /* !VECTOR_H_ */
