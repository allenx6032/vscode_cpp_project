/*
** EPITECH PROJECT, 2021
** str_basic
** File description:
** The base of str
*/

#include <unistd.h>
#include <stdlib.h>
#include "engine_teklibc.h"

int my_strlen(const char *str)
{
    int result = 0;
    for (; str[result] != '\0'; result++);
    return result;
}

void my_putstr(const char *str)
{
    write(1, str, my_strlen(str));
}

char *my_strrev(char *str)
{
    char *result = my_strdup(str);
    int len = my_strlen(result);

    for (int i = 0; i != len; i++)
        result[i] =  str[len - i - 1];
    return result;
}

char *my_strdup(const char *src)
{
    int len = my_strlen(src) + 1;
    char *result = malloc(sizeof(char) * len);

    for (int i = 0; i != len; i++)
        result[i] = src[i];
    return result;
}

int my_strcmp(const char *str1, const char *str2)
{
    int i = 0;

    while (str1[i] == str2[i]) {
        if (str1[i] == '\0')
            return 0;
        i++;
    }
    return str1[i] - str2[i];
}