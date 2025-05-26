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

void medic_create(world_entity_t *entity, sfVector2f pos)
{
    medic_t *medic = malloc(sizeof(medic_t));

    if (medic == NULL)
        return;
    entity_create(&entity->all_entity, &medic->entity_data,
        MEDIC_ENTITY, pos);
    medic->regen_clock = sfClock_create();
    my_vector_add(&entity->medic, medic);
}

void medic_init(vector_t *medic_vec)
{
    my_vector_init(medic_vec);
}

void medic_update(world_entity_t *entity, game_t *game)
{
    medic_t *current_medic;

    for (int i = 0; i != entity->medic.size; i++) {
        current_medic = my_vector_get(&entity->medic, i);
        if (sfvector2f_getdistance(current_medic->entity_data.position,
            game->player.position) < 40)
        {
            if (game->player.life < 100 && sfTime_asSeconds(
                    sfClock_getElapsedTime(current_medic->regen_clock)) >= 1) {
                game->player.life += 2;
                sfClock_restart(current_medic->regen_clock);
            }
        }
    }
}

void medic_render(sfRenderWindow *window, world_entity_t *entity)
{
}

void medic_destroy(world_entity_t *world_entity)
{
    medic_t *current_medic;

    for (int i = 0; i != world_entity->medic.size; i++) {
        current_medic = my_vector_get(&world_entity->medic, i);
        sfClock_destroy(current_medic->regen_clock);
        entity_delete(&world_entity->all_entity, &current_medic->entity_data);
        free(current_medic);
    }
    my_vector_free(&world_entity->medic);
}