/*
** EPITECH PROJECT, 2023
** my_strcpy
** File description:
** Write a function that copies a string into another.
** The destination string will already have enough memory
** to copy the source string
*/

#include "my.h"

char *my_strcpy(char *dest, char *src)
{
    int len = my_strlen(src);
    int i = 0;

    for (; i < len; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}
