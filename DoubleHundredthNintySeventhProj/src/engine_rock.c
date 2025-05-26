/*
** EPITECH PROJECT, 2021
** rock
** File description:
** Rock building for RPG
*/

#include "engine_building.h"
#include <stdlib.h>

void rock_create(world_building_t *building, sfVector2f pos)
{
    rock_t *rock = malloc(sizeof(rock_t));

    if (rock == NULL)
        return;
    building_create(&building->all_building, &rock->building_data,
        ROCK_BUILDING, pos);
    rock->building_data.passable = false;
    my_vector_add(&building->rock, rock);
}

void rock_init(vector_t *rock_vec)
{
    my_vector_init(rock_vec);
}

void rock_update(world_building_t *building)
{
    rock_t *current_rock;

    for (int i = 0; i != building->rock.size; i++) {
        current_rock = my_vector_get(&building->rock, i);
    }
}

void rock_render(sfRenderWindow *window, world_building_t *building)
{
    rock_t *current_rock;

    for (int i = 0; i != building->rock.size; i++) {
        current_rock = my_vector_get(&building->rock, i);
    }
}

void rock_destroy(world_building_t *world_building)
{
    rock_t *current_rock;

    for (int i = 0; i != world_building->rock.size; i++) {
        current_rock = my_vector_get(&world_building->rock, i);
        building_delete(&world_building->all_building,
            &current_rock->building_data);
        free(current_rock);
    }
    my_vector_free(&world_building->rock);
}