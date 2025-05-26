/*
** EPITECH PROJECT, 2023
** libmy
** File description:
** dupplicate an array
*/

#include <stdlib.h>
#include "my_utils.h"
#include "my.h"

char *my_strdup(char const *src)
{
    int i;
    char *to_return;

    if (my_strlen(src) < 1)
        return malloc_empty(sizeof(char));
    to_return = malloc_empty(sizeof(char) * (my_strlen(src) + 1));
    for (i = 0; i <= my_strlen(src); i++)
        to_return[i] = src[i];
    return to_return;
}
