/*
** EPITECH PROJECT, 2021
** str_mem
** File description:
** Even more str things
*/

#include <stdio.h>
#include <stdlib.h>
#include "engine_teklibc.h"

char *my_str_uper(char *str)
{
    int len = my_strlen(str);

    for (int i = 0; i != len; i++)
        str[i] = my_char_uper(str[i]);
    return str;
}

char *my_str_lower(char *str)
{
    int len = my_strlen(str);

    for (int i = 0; i != len; i++)
        str[i] = my_char_lower(str[i]);
    return str;
}

bool my_strcontains(const char *input, const char *check)
{
    int len = my_strlen(check);
    int input_len = my_strlen(input);
    int current_progess = 0;

    for (int i = 0; i != input_len; i++) {
        if (input[i] == check[current_progess]) {
            current_progess++;
            if (current_progess == len)
                return 1;
        } else {
            current_progess = 0;
        }
    }
    return 0;
}

bool my_str_is_null_or_empty(const char *str)
{
    int len;

    if (str == NULL)
        return true;
    len = my_strlen(str);
    if (len == 0)
        return true;
    for (int i = 0; i != len; i++)
        if (str[i] != ' ')
            return false;
    return true;
}

char *my_strcat(const char *init, const char *to_add)
{
    char *str = malloc(sizeof(char) *
        (my_strlen(to_add) + my_strlen(init) + 1));
    int org_size = my_strlen(init);
    int add_size = my_strlen(to_add);

    for (int i = 0; init[i] != '\0'; i++)
        str[i] = init[i];
    for (int i = 0; i != add_size + 1; i++)
        str[org_size + i] = to_add[i];
    return str;
}