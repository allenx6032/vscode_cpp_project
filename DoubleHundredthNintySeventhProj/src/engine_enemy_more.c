/*
** EPITECH PROJECT, 2021
** enemy_more
** File description:
** enemy_more
*/

#include "engine_entity.h"
#include "engine_sfml_extra.h"
#include "engine_rpg.h"
#include <stdlib.h>

void enemy_create(world_entity_t *entity, sfVector2f pos)
{
    enemy_t *enemy = malloc(sizeof(enemy_t));

    if (enemy == NULL)
        return;
    entity_create(&entity->all_entity, &enemy->entity_data,
        ENEMY_ENTITY, pos);
    enemy->damage_clock = sfClock_create();
    enemy->move_clock = sfClock_create();
    my_vector_add(&entity->enemy, enemy);
}

void remove_enemy(world_entity_t *world_entity, entity_t entity)
{
    enemy_t *current_enemy;

    for (int i = 0; i != world_entity->enemy.size; i++) {
        current_enemy = my_vector_get(&world_entity->enemy, i);
        if (current_enemy->entity_data.sprite == entity.sprite) {
            current_enemy->entity_data.visible = false;
        }
    }
}