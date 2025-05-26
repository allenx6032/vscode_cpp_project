/*
** EPITECH PROJECT, 2023
** libmy
** File description:
** cpy src at the end of dest
*/

#include "my_utils.h"

char *my_strcat(char *dest, const char *src)
{
    int dest_len = my_strlen(dest);
    int i;

    for (i = 0 ; src[i] != '\0' ; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';

    return dest;
}
