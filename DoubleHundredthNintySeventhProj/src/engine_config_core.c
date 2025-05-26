/*
** EPITECH PROJECT, 2021
** config
** File description:
** Config for RPG (Supported Config : .INI/.CONF/.CFG)
*/

/*
    TODO :  Support section, Perfectly free, config_set need to be reworked
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "engine_teklibc.h"
#include "engine_config.h"

void config_parse(config_t *config)
{
    int helper = 0;
    char *dup = NULL;

    for (int i = 0; config->file_array[i] != NULL; i += 1)
        if (my_strcontains(config->file_array[i], "="))
            helper += 1;
    config->all_values = malloc(sizeof(config_data_t *) * (helper + 1));
    helper = 0;
    for (int i = 0; config->file_array[i] != NULL; i += 1) {
        if (my_strcontains(config->file_array[i], "=")) {
            dup = my_strdup(config->file_array[i]);
            config->all_values[helper] = malloc(sizeof(config_data_t));
            config->all_values[helper]->key =
                my_strtok_r(config->file_array[i], '=', &dup);
            config->all_values[helper]->value = my_strdup(dup);
            helper += 1;
        }
    }
    config->all_values[helper] = NULL;
}

config_t *config_load(char *path)
{
    config_t *config = malloc(sizeof(config_t));
    char *dup;
    int helper = 0;

    if (my_str_is_null_or_empty(path))
        return NULL;
    config->file_str = my_fread(path);
    if (my_str_is_null_or_empty(config->file_str))
        config->file_str = "";
    dup = my_strdup(config->file_str);
    for (helper = 0; my_strtok_r(dup, '\n', &dup) != NULL; helper += 1);
    config->file_array = malloc(sizeof(char *) * (helper + 1));
    dup = my_strdup(config->file_str);
    for (int i = 0; i != helper; i++)
        config->file_array[i] = my_strtok_r(dup, '\n', &dup);
    config->file_array[helper] = NULL;
    config_parse(config);
    return config;
}

int config_save(const config_t *config, char *path)
{
    FILE *fptr = NULL;
    char *builder = NULL;

    fptr = fopen(path, "w");
    if (fptr == NULL)
        return 0;
    for (int i = 0; config->all_values[i] != NULL; i++) {
        if (!my_str_is_null_or_empty(config->all_values[i]->key)
            && !my_str_is_null_or_empty(config->all_values[i]->value)) {
            builder = my_strcat(config->all_values[i]->key,
                    my_strcat("=", config->all_values[i]->value));
            fwrite(builder, sizeof(char) * my_strlen(builder), 1, fptr);
            fwrite("\n", sizeof(char), 1, fptr);
        }
    }
    fclose(fptr);
    return 1;
}

void config_unload(config_t *config)
{
    free(config->file_str);
    for (int i = 0; config->file_array[i] != NULL; i++)
        free(config->file_array[i]);
    for (int i = 0; config->all_values[i] != NULL; i++) {
        if (config->all_values[i]->key != NULL)
            free(config->all_values[i]->key);
        if (config->all_values[i]->value != NULL)
            free(config->all_values[i]->value);
    }
    free(config);
    config = NULL;
}