/*
** EPITECH PROJECT, 2021
** particule_more
** File description:
** particule_more
*/

#include "engine_particule.h"
#include <stdlib.h>

void particule_create(vector_t *all_particule, particule_t *particule,
    particule_type type, sfVector2f pos)
{
    sfVector2f position_fixed = pos;

    if (particule == NULL)
        return;
    particule->type = type;
    particule->visible = true;
    particule->sprite = sfSprite_create();
    particule->texture = particule_get_texture(type);
    particule->loop = false;
    particule->anim_clock = sfClock_create();
    sfSprite_setTextureRect(particule->sprite,
        particule_get_default_texture_rec(type));
    position_fixed.x -= sfSprite_getTextureRect(particule->sprite).width / 2;
    position_fixed.y -= sfSprite_getTextureRect(particule->sprite).height / 2;
    particule->position = position_fixed;
    sfSprite_setPosition(particule->sprite, particule->position);
    my_vector_add(all_particule, particule);
}

void particule_delete(vector_t *all_particule, particule_t *particule)
{
    particule_t *current_particule;

    for (int i = 0; i != all_particule->size; i++) {
        current_particule = my_vector_get(all_particule, i);
        if (current_particule == particule) {
            sfSprite_destroy(current_particule->sprite);
            sfTexture_destroy(current_particule->texture);
            sfClock_destroy(current_particule->anim_clock);
            my_vector_delete(all_particule, i);
            return;
        }
    }
}

sfTexture *particule_get_texture(particule_type type)
{
    if (type == HEAL_PARTICULE)
        return sfTexture_createFromFile("./data/particule/heal.png", NULL);
    return NULL;
}

sfIntRect particule_get_default_texture_rec(particule_type type)
{
    if (type == HEAL_PARTICULE)
        return (sfIntRect) { 0, 0, 32, 30 };
    return (sfIntRect) { 0, 0, 0, 0 };
}