/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** get the save file
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_utils.h"

static char *my_str_concat_path(char const *nb1, char const *nb2)
{
    char *to_return;

    if (nb1 == NULL && nb2 == NULL)
        return NULL;
    if (nb1 == NULL)
        return my_strdup(nb2);
    if (nb2 == NULL)
        return my_strdup(nb1);
    to_return = malloc_empty(sizeof(char) *
        (my_strlen(nb1) + my_strlen(nb2) + 2));
    my_strcpy(to_return, nb1);
    to_return[my_strlen(nb1)] = '/';
    my_strcpy(&to_return[my_strlen(nb1) + 1], nb2);
    to_return[my_strlen(nb1) + my_strlen(nb2) + 1] = '\0';
    return to_return;
}

static char *find_elem(char *const *env, char const *name)
{
    unsigned int j;

    for (unsigned int i = 0; env[i] != NULL; i++) {
        j = 0;
        while (env[i][j] != '=') {
            j++;
        }
        if (!my_strncmp(env[i], name, j)) {
            return &env[i][j + 1];
        }
    }
    return NULL;
}

FILE *get_save(char **env)
{
    char *name = find_elem(env, "HOME");
    FILE *file = NULL;

    if (name == NULL)
        name = ".";
    name = my_str_concat_path(name, ".jérémyrpg_.saves ?_coded.nap");
    file = fopen(name, "r+");
    if (file == NULL) {
        file = fopen(name, "w+");
        if (file != NULL) {
            fclose(file);
            file = fopen(name, "r+");
        }
    }
    free(name);
    if (file == NULL)
        write(2, "Save : Cannot open file.", 24);
    return file;
}
