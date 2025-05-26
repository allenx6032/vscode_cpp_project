/*
** EPITECH PROJECT, 2021
** key_more
** File description:
** More of Key Building
*/

#include <stdlib.h>
#include "engine_teklibc.h"
#include "engine_rpg.h"

void key_update_config(game_t *game, key_b_t *current_key)
{
    if (config_key_exist(game->config, "inv_key")) {
        if (my_atoi(config_get(game->config, "inv_key")) == 1){
            game->player.items.key = true;
            current_key->building_data.visible = false;
        } else {
            game->player.items.key = false;
            current_key->building_data.visible = true;
        }
    } else {
        game->player.items.key = false;
        current_key->building_data.visible = true;
    }
}