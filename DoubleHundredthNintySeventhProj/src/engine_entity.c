/*
** EPITECH PROJECT, 2021
** entity
** File description:
** Simple Entity Base for any entity
*/

#include "engine_rpg.h"
#include "engine_entity.h"
#include "engine_sfml_extra.h"

void entity_init(world_entity_t *entity)
{
    my_vector_init(&entity->all_entity);
    medic_init(&entity->medic);
    enemy_init(&entity->enemy);
}

void entity_update(world_entity_t *entity, game_t *game)
{
    entity_t *current_entity;

    medic_update(entity, game);
    enemy_update(game, entity);
    for (int i = 0; i != entity->all_entity.size; i++) {
        current_entity = my_vector_get(&entity->all_entity, i);
        if (current_entity->life <= 0) {
            if (current_entity->type == ENEMY_ENTITY)
                remove_enemy(entity, *current_entity);
        }
        sfSprite_setTexture(current_entity->sprite,
            current_entity->texture, sfFalse);
        sfSprite_setPosition(current_entity->sprite,
            current_entity->position);
    }
}

void entity_render(sfRenderWindow *window, world_entity_t *entity)
{
    entity_t *current_entity;

    for (int i = 0; i != entity->all_entity.size; i++) {
        current_entity = my_vector_get(&entity->all_entity, i);
        if (current_entity->visible) {
            sfRenderWindow_drawSprite(window,
                current_entity->sprite, NULL);
        }
    }
}

void entity_destroy(world_entity_t *entity)
{
    entity_t *current_entity;

    medic_destroy(entity);
    enemy_destroy(entity);
    for (int i = 0; i != entity->all_entity.size; i++) {
        current_entity = my_vector_get(&entity->all_entity, i);
        sfSprite_destroy(current_entity->sprite);
        sfTexture_destroy(current_entity->texture);
    }
    my_vector_free(&entity->all_entity);
}

void damage_entity_arrond(world_entity_t *entity,
    sfVector2f pos, int dist, int damage)
{
    entity_t *current_entity;

    for (int i = 0; i != entity->all_entity.size; i++) {
        current_entity = my_vector_get(&entity->all_entity, i);
        if (current_entity->type == ENEMY_ENTITY) {
            if (sfvector2f_getdistance(pos, current_entity->position)
                <= dist) {
                current_entity->life -= damage;
            }
        }
    }
}