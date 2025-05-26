/*
** EPITECH PROJECT, 2023
** str dup
** File description:
** cdd
*/
#include <stdlib.h>
#include <stdio.h>
#include "my.h"

char *my_strdup(char *src)
{
    int len = my_strlen(src);
    char *nv = malloc(sizeof(char) * (len + 1));

    if (nv == NULL)
        return NULL;
    my_strcpy(nv, src);
    return nv;
}
