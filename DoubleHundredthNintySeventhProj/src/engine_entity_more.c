/*
** EPITECH PROJECT, 2021
** entity_more
** File description:
** Entity Infos
*/

#include "engine_entity.h"

void entity_create(vector_t *all_entity, entity_t *entity,
    entity_type type, sfVector2f pos)
{
    sfVector2f position_fixed = pos;

    entity->damage = 0;
    entity->friendly = (entity->type == ENEMY_ENTITY) ? false : true;
    entity->invulnerable = true;
    entity->life = 100;
    entity->type = type;
    entity->sprite = sfSprite_create();
    entity->texture = entity_get_texture(type);
    entity->visible = true;
    sfSprite_setTextureRect(entity->sprite,
        entity_get_default_texture_rec(type));
    position_fixed.x -= sfSprite_getTextureRect(entity->sprite).width / 2;
    position_fixed.y -= sfSprite_getTextureRect(entity->sprite).height / 2;
    entity->position = position_fixed;
    sfSprite_setPosition(entity->sprite, entity->position);
    my_vector_add(all_entity, entity);
}

void entity_delete(vector_t *all_entity, entity_t *entity)
{
    entity_t *current_entity;

    for (int i = 0; i != all_entity->size; i++) {
        current_entity = my_vector_get(all_entity, i);
        if (current_entity == entity) {
            sfSprite_destroy(current_entity->sprite);
            sfTexture_destroy(current_entity->texture);
            my_vector_delete(all_entity, i);
            return;
        }
    }
}

sfTexture *entity_get_texture(entity_type type)
{
    if (type == MEDIC_ENTITY)
        return sfTexture_createFromFile("./data/entity/medic.png", NULL);
    if (type == ENEMY_ENTITY)
        return sfTexture_createFromFile("./data/entity/enemie_1.png", NULL);
    return NULL;
}

sfIntRect entity_get_default_texture_rec(entity_type type)
{
    if (type == MEDIC_ENTITY)
        return (sfIntRect) { 32, 0, 32, 32 };
    if (type == ENEMY_ENTITY)
        return (sfIntRect) { 32, 0, 32, 32 };
    return (sfIntRect) { 0, 0, 0, 0 };
}