/*
** EPITECH PROJECT, 2021
** key
** File description:
** Key Item Building (yep, i know...)
*/

#include <stdlib.h>
#include "engine_building.h"
#include "engine_teklibc.h"
#include "engine_sfml_extra.h"
#include "engine_rpg.h"

void key_create(world_building_t *building, sfVector2f pos)
{
    key_b_t *key = malloc(sizeof(key_b_t));

    if (key == NULL)
        return;
    building_create(&building->all_building, &key->building_data,
        KEY_BUILDING, pos);
    key->building_data.passable = true;
    my_vector_add(&building->key, key);
}

void key_init(vector_t *key_vec)
{
    my_vector_init(key_vec);
}

void key_update(game_t *game, world_building_t *building)
{
    key_b_t *current_key;

    for (int i = 0; i != building->key.size; i++) {
        current_key = my_vector_get(&building->key, i);
        if (sfsprite_isspriteinside(game->player.sprite,
            current_key->building_data.sprite)) {
            if (game->player.items.key == false) {
                current_key->building_data.visible = false;
                sound_play(&game->sounds, NEW_ITEM_SOUND, sfFalse,
                    (float) my_atoi(config_get(game->config, "volume")));
                config_set(game->config, "inv_key", "1");
            }
        }
        key_update_config(game, current_key);
    }
}

void key_render(sfRenderWindow *window, world_building_t *building)
{
    key_b_t *current_key;

    for (int i = 0; i != building->key.size; i++) {
        current_key = my_vector_get(&building->key, i);
    }
}

void key_destroy(world_building_t *world_building)
{
    key_b_t *current_key;

    for (int i = 0; i != world_building->key.size; i++) {
        current_key = my_vector_get(&world_building->key, i);
        building_delete(&world_building->all_building,
            &current_key->building_data);
        free(current_key);
    }
    my_vector_free(&world_building->key);
}