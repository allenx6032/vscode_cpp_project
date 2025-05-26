/*
** EPITECH PROJECT, 2021
** char_basic
** File description:
** char_basic
*/

#include <unistd.h>

void my_putchar(const char c)
{
    write(1, &c, 1);
}

char my_char_uper(char c)
{
    if (c >= 'a' && c <= 'z')
        c = c + ('A' - 'a');
    return c;
}

char my_char_lower(char c)
{
    if (c >= 'A' && c <= 'Z')
        c = c + ('a' - 'A');
    return c;
}