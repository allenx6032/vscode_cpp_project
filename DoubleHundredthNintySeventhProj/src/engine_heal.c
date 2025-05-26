/*
** EPITECH PROJECT, 2021
** heal
** File description:
** Heal Effect to regen player life
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_particule.h"
#include "engine_sfml_extra.h"
#include "engine_rpg.h"
#include <stdlib.h>

void heal_create(world_particule_t *particule, sfVector2f pos)
{
    heal_t *heal = malloc(sizeof(rock_t));

    if (heal == NULL)
        return;
    particule_create(&particule->all_particule, &heal->particule_data,
        HEAL_PARTICULE, pos);
    my_vector_add(&particule->heal, heal);
}

void heal_init(vector_t *heal_vec)
{
    my_vector_init(heal_vec);
}

void heal_update(world_particule_t *particule)
{
    heal_t *current_heal;
    sfIntRect heal_rec;

    for (int i = 0; i != particule->heal.size; i++) {
        current_heal = my_vector_get(&particule->heal, i);
        if (sfTime_asSeconds(sfClock_getElapsedTime(
            current_heal->particule_data.anim_clock)) > 0.2) {
            heal_animate(particule, current_heal);
            return;
        }
    }
}

void heal_render(sfRenderWindow *window, world_particule_t *particule)
{
}

void heal_destroy(world_particule_t *world_particule)
{
    heal_t *current_heal;

    while (world_particule->heal.size != 0) {
        current_heal = my_vector_get(&world_particule->heal,
            world_particule->heal.size - 1);
        heal_delete(world_particule, current_heal);
    }
    my_vector_free(&world_particule->heal);
}