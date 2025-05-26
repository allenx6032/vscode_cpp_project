/*
** EPITECH PROJECT, 2021
** mountains
** File description:
** Mountains Building for RPG
*/

#include "engine_building.h"
#include <stdlib.h>

void mountains_create(world_building_t *building, sfVector2f pos)
{
    mountains_t *mountains = malloc(sizeof(mountains_t));

    if (mountains == NULL)
        return;
    building_create(&building->all_building, &mountains->building_data,
        MOUNTAINS_BUILDING, pos);
    mountains->building_data.passable = false;
    my_vector_add(&building->mountains, mountains);
}

void mountains_init(vector_t *mountains_vec)
{
    my_vector_init(mountains_vec);
}

void mountains_update(world_building_t *building)
{
    mountains_t *current_mountains;

    for (int i = 0; i != building->mountains.size; i++) {
        current_mountains = my_vector_get(&building->mountains, i);
    }
}

void mountains_render(sfRenderWindow *window, world_building_t *building)
{
    mountains_t *current_mountains;

    for (int i = 0; i != building->mountains.size; i++) {
        current_mountains = my_vector_get(&building->mountains, i);
    }
}

void mountains_destroy(world_building_t *world_building)
{
    mountains_t *current_mountains;

    for (int i = 0; i != world_building->mountains.size; i++) {
        current_mountains = my_vector_get(&world_building->mountains, i);
        building_delete(&world_building->all_building,
            &current_mountains->building_data);
        free(current_mountains);
    }
    my_vector_free(&world_building->mountains);
}