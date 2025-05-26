/*
** EPITECH PROJECT, 2021
** config_manager
** File description:
** Config Manager for RPG
*/

#include <unistd.h>
#include <stdlib.h>
#include "engine_teklibc.h"
#include "engine_config.h"

char *config_get(const config_t *config, char *key)
{
    for (int i = 0; config->all_values[i] != NULL; i++) {
        if (my_strcmp(config->all_values[i]->key, key) == 0)
            return config->all_values[i]->value;
    }
    return NULL;
}

int config_key_exist(const config_t *config, char *key)
{
    for (int i = 0; config->all_values[i] != NULL; i++) {
        if (my_strcmp(config->all_values[i]->key, key) == 0)
            return 1;
    }
    return 0;
}

int config_is_value_defined(const config_t *config, char *key)
{
    for (int i = 0; config->all_values[i] != NULL; i++) {
        if (my_strcmp(config->all_values[i]->key, key) == 0) {
            if (my_str_is_null_or_empty(config->all_values[i]->value))
                return 0;
            return 1;
        }
    }
    return 0;
}

void config_set(config_t *config, char *key, char *value)
{
    int helper = 0;

    for (helper = 0; config->all_values[helper] != NULL; helper++) {
        if (my_strcmp(config->all_values[helper]->key, key) == 0) {
            config->all_values[helper]->value = value;
            return;
        }
    }
    config_data_t **new_data = malloc(sizeof(config_data_t) * (helper + 1));
    for (int i = 0; config->all_values[i] != NULL; i++)
        new_data[i] = config->all_values[i];
    new_data[helper] = malloc(sizeof(config_data_t));
    new_data[helper]->key = my_strdup(key);
    new_data[helper]->value = my_strdup(value);
    new_data[helper + 1] = NULL;
    config->all_values = new_data;
}