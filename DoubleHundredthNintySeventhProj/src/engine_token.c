/*
** EPITECH PROJECT, 2021
** token
** File description:
** my_strtok, not perfect but (a little) proud of it
*/

#include <stdlib.h>
#include "engine_teklibc.h"

char *my_strtok(char *str, char delim)
{
    int helper = 0;
    int next = 0;
    char *build = NULL;

    if (my_str_is_null_or_empty(str))
        return NULL;
    for (next = 0; str[next] != '\0'; next++)
        if (str[next] == delim)
            helper++;
    if (helper == 0)
        return NULL;
    for (next = 0; str[next] != delim; next++)
        if (str[next] == delim)
            helper++;
    build = malloc(sizeof(char) * (next + 1));
    helper = 0;
    for (helper = 0; helper != (next); helper++)
        build[helper] = str[helper];
    build[helper] = '\0';
    return build;
}

char *my_strtok_r(char *str, char delim, char **saveptr)
{
    int helper = 0;
    int next = 0;
    char *token = my_strtok(str, delim);

    if (my_str_is_null_or_empty(str))
        return NULL;
    for (next = 0; str[next] != '\0'; next++)
        if (str[next] == delim)
            helper++;
    if (helper == 0) {
        *saveptr = NULL;
        return str;
    }
    for (next = 0; str[next] != delim; next++)
        if (str[next] == delim)
            helper++;
    *saveptr = str;
    for (helper = 0; helper != (next + 1); helper++)
        (*saveptr)++;
    return token;
}