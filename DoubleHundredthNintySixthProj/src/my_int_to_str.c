/*
** EPITECH PROJECT, 2023
** MUL_my_hunter_
** File description:
** int_to_str.c
*/
#include <stdlib.h>
#include <stdio.h>
#include "my.h"

char *my_revert(char *str)
{
    int i = 0;
    int j = my_strlen(str) - 1;
    char *tmp = malloc(sizeof(char) * (my_strlen(str) + 1));

    for (; str[i] != '\0'; i++) {
        tmp[i] = str[j];
        j--;
    }
    tmp[i] = '\0';
    return tmp;
}

char *tostr(long long int nb)
{
    int i = 0;
    char *str = malloc(sizeof(char) * 10);

    while (1) {
        if (nb <= 9) {
            str[i] = nb + 48;
            str[i + 1] = '\0';
            str = my_revert(str);
            return str;
        }
        str[i] = (nb % 10) + 48;
        nb /= 10;
        i++;
        }
    return str;
}

// int main(void)
// {
//     printf("%s\n", my_int_to_str(0));
//     return 0;
// }
