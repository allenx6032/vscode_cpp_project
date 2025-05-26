/*
** EPITECH PROJECT, 2021
** particule
** File description:
** Particule basic calls for RPG
*/

/*
    TODO: Revérifier les malloc et free
*/

#include <stdlib.h>
#include "engine_particule.h"

void particule_init(world_particule_t *particule)
{
    my_vector_init(&particule->all_particule);
    heal_init(&particule->heal);
}

void particule_update(world_particule_t *particule)
{
    particule_t *current_particule;

    heal_update(particule);
    for (int i = 0; i != particule->all_particule.size; i++) {
        current_particule = my_vector_get(&particule->all_particule, i);
        sfSprite_setTexture(current_particule->sprite,
            current_particule->texture, sfFalse);
        sfSprite_setPosition(current_particule->sprite,
            current_particule->position);
    }
}

void particule_render(sfRenderWindow *window, world_particule_t *particule)
{
    particule_t *current_particule;

    heal_render(window, particule);
    for (int i = 0; i != particule->all_particule.size; i++) {
        current_particule = my_vector_get(&particule->all_particule, i);
        if (current_particule->visible) {
            sfRenderWindow_drawSprite(window,
                current_particule->sprite, NULL);
        }
    }
}

void particule_destroy(world_particule_t *particule)
{
    particule_t *current_particule;

    heal_destroy(particule);
    for (int i = 0; i != particule->all_particule.size; i++) {
        current_particule = my_vector_get(&particule->all_particule, i);
        sfSprite_destroy(current_particule->sprite);
        sfTexture_destroy(current_particule->texture);
        sfClock_destroy(current_particule->anim_clock);
        free(current_particule);
    }
    my_vector_free(&particule->all_particule);
}