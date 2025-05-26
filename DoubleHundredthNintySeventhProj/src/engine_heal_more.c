/*
** EPITECH PROJECT, 2021
** heal_more
** File description:
** heal_more
*/

#include "engine_particule.h"
#include <stdlib.h>

void heal_animate(world_particule_t *particule, heal_t *heal)
{
    sfIntRect heal_rec = sfSprite_getTextureRect(
        heal->particule_data.sprite);

    if (heal_rec.left != 160)
        heal_rec.left += 32;
    else
        heal_rec.left = 0;
    if (heal_rec.left == 0 && !heal->particule_data.loop) {
        heal_delete(particule, heal);
    } else {
        sfSprite_setTextureRect(heal->particule_data.sprite,
            heal_rec);
        sfClock_restart(heal->particule_data.anim_clock);
    }
}

void heal_delete(world_particule_t *particule, heal_t *heal)
{
    heal_t *current_heal;

    for (int i = 0; i != particule->heal.size; i++) {
        current_heal = my_vector_get(&particule->heal, i);
        if (current_heal == heal) {
            particule_delete(&particule->all_particule,
                &current_heal->particule_data);
            my_vector_delete(&particule->heal, i);
            free(current_heal);
            return;
        }
    }
}