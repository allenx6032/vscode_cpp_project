/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** save_only
*/

#ifndef SAVE_ONLY_H_
    #define SAVE_ONLY_H_

    #include <stdio.h>
    #include "my.h"

FILE *get_save(char **env);
bool test_corruption(char_info_t *perso, char_info_t *perso_error);

#endif
