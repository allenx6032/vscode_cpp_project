/*
** EPITECH PROJECT, 2023
** my_put_nbr
** File description:
** Write a function that displays
** the number given
** as a parameter. It must be able to display
** all the possible
** values of an int, and must be prototyped as follows:
*/
#include <stdio.h>
#include "my.h"

int my_put_nbr_rec(long long int nb, int a)
{
    int m = 0;

    if (nb <= 9) {
        my_putchar(nb + '0');
        a++;
        return a;
    }
    m = nb % 10;
    a++;
    a = my_put_nbr_rec(nb / 10, a);
    my_putchar(m + '0');
    return a;
}

int my_put_nbr(long long int nb)
{
    int a = 0;

    if (nb < 0) {
        my_putchar('-');
        nb = -nb;
        a++;
    }
    return my_put_nbr_rec(nb, a);
}
