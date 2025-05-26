/*
** EPITECH PROJECT, 2021
** int_basic
** File description:
** int_basic
*/

#include <stdlib.h>
#include <unistd.h>
#include "engine_teklibc.h"

int my_atoi(const char *str)
{
    int len = my_strlen(str);
    int result = 0;
    int tmp = 0;

    while (tmp != len) {
        result = (result * 10) + (str[tmp] - 48);
        tmp++;
    }
    return result;
}

int my_intlen(int nb)
{
    int len = 1;

    while (nb >= 10) {
        nb /= 10;
        len++;
    }
    return len;
}

char *my_itoa(int nb)
{
    bool neg = nb < 0;
    char *result;
    char *conversion;
    int len = 0;

    if (neg)
        nb *= -1;
    conversion = malloc(sizeof(char) * (my_intlen(nb) + 1 + neg));
    if (nb == 0)
        conversion[len++] = '0';
    while (nb != 0) {
        conversion[len++] = (nb % 10) + '0';
        nb /= 10;
    }
    if (neg)
        conversion[len++] = '-';
    conversion[len] = '\0';
    result = my_strrev(conversion);
    free(conversion);
    return result;
}

void my_putint(const int nb)
{
    char *result = my_itoa(nb);

    write(1, result, my_intlen(nb));
    free(result);
}