/*
** EPITECH PROJECT, 2021
** args
** File description:
** A simple way to check args and values
*/

#include "engine_teklibc.h"

bool arg_exist(int ac, char **av, char *arg)
{
    if (ac > 1) {
        for (int i = 1; i != ac; i++) {
            if (my_strcmp(av[i], arg) == 0)
                return true;
        }
    }
    return false;
}