/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#include "my.h"

char *my_strlowcase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 65 && str[i] <= 90)
            str[i] = str[i] + 32;
    }
    return str;
}
