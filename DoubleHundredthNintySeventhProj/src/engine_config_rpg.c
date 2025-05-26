/*
** EPITECH PROJECT, 2021
** config_rpg
** File description:
** Easy config for RPG (only)
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"

void check_and_create_config(config_t *config,
    char *key, int default_value, int max)
{
    int tmp = 0;

    if (!config_key_exist(config, key))
        config_set(config, key, my_itoa(default_value));
    else {
        tmp = my_atoi(config_get(config, key));
        if (tmp < 0 || tmp > max)
            config_set(config, key, my_itoa(default_value));
    }
}

void load_rpg_config(config_t *config)
{
    check_and_create_config(config, "volume", 80, 100);
    check_and_create_config(config, "fps", 60, 120);
    check_and_create_config(config, "fullscreen", 0, 1);
}
