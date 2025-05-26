/*
** EPITECH PROJECT, 2023
** my_putstr
** File description:
** Write a function that displays, one-by-one, the characters of a string.
** The address of the string’s first character will be found in
** the pointer passed as a parameter to the function
*/
#include "my.h"

int my_putstr(char const *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        my_putchar(str[i]);
    }
    return 0;
}
