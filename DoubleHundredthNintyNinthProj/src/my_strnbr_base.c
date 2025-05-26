/*
** EPITECH PROJECT, 2023
** libmy
** File description:
** putnbr into string depends on a given base
*/

#include <stdlib.h>
#include "my_utils.h"

static void my_strnbr_base_part2_into_string(char *ret, int nbr,
    char const *base)
{
    int len = my_strlen(base);
    char nb;

    if (nbr >= len) {
        nb = nbr % len;
        my_strnbr_base_part2_into_string(ret, (nbr - nb) / len, base);
    } else {
        nb = nbr;
    };
    ret[my_strlen(ret)] = base[(unsigned char) nb];
}

char *my_strnbr_base(int nbr, char const *base)
{
    char *ret = malloc_empty(10);

    if (nbr < 0) {
        ret[my_strlen(ret)] = '-';
        nbr = 0 - nbr;
    };
    if (nbr == 0) {
        ret[my_strlen(ret)] = base[0];
        return ret;
    };
    my_strnbr_base_part2_into_string(ret, nbr, base);
    return ret;
}
