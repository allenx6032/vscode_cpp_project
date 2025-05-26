/*
** EPITECH PROJECT, 2021
** medic
** File description:
** Medical Entity to regen player life
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_entity.h"
#include "engine_sfml_extra.h"
#include "engine_rpg.h"
#include <stdlib.h>

void enemy_init(vector_t *enemy_vec)
{
    my_vector_init(enemy_vec);
}

void enemy_update(game_t *game, world_entity_t *entity)
{
    enemy_t *current_enemy;

    for (int i = 0; i != entity->enemy.size; i++) {
        current_enemy = my_vector_get(&entity->enemy, i);
        if (sfvector2f_getdistance(current_enemy->entity_data.position,
            game->player.position) < 40)
        {
            if (game->player.life > 0 && sfTime_asSeconds(
                    sfClock_getElapsedTime(current_enemy->damage_clock)) >= 1) {
                game->player.life -= 10;
                sfClock_restart(current_enemy->damage_clock);
            }
        }
    }
}

void enemy_render(sfRenderWindow *window, world_entity_t *entity)
{
}

void enemy_destroy(world_entity_t *world_entity)
{
    enemy_t *current_enemy;

    for (int i = 0; i != world_entity->enemy.size; i++) {
        current_enemy = my_vector_get(&world_entity->enemy, i);
        sfClock_destroy(current_enemy->damage_clock);
        sfClock_destroy(current_enemy->move_clock);
        entity_delete(&world_entity->all_entity, &current_enemy->entity_data);
        free(current_enemy);
    }
    my_vector_free(&world_entity->enemy);
}