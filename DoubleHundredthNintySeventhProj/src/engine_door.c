/*
** EPITECH PROJECT, 2021
** door
** File description:
** door building for RPG
*/

#include "engine_building.h"
#include "engine_sfml_extra.h"
#include "engine_rpg.h"
#include <stdlib.h>

void door_create(world_building_t *building, sfVector2f pos)
{
    door_t *door = malloc(sizeof(door_t));

    if (door == NULL)
        return;
    building_create(&building->all_building, &door->building_data,
        DOOR_BUILDING, pos);
    door->building_data.passable = false;
    my_vector_add(&building->door, door);
}

void door_init(vector_t *door_vec)
{
    my_vector_init(door_vec);
}

void door_update(world_building_t *building)
{
    door_t *current_door;

    for (int i = 0; i != building->door.size; i++) {
        current_door = my_vector_get(&building->door, i);
    }
}

void door_render(sfRenderWindow *window, world_building_t *building)
{
    door_t *current_door;

    for (int i = 0; i != building->door.size; i++) {
        current_door = my_vector_get(&building->door, i);
        if (current_door->state == DOOR_CLOSED)
            sfSprite_setTextureRect(current_door->building_data.sprite,
                (sfIntRect) { 1196, 182, 54, 60 });
        if (current_door->state == DOOR_OPEN)
            sfSprite_setTextureRect(current_door->building_data.sprite,
                (sfIntRect) { 1196, 326, 54, 60 });
    }
}

void door_destroy(world_building_t *world_building)
{
    door_t *current_door;

    for (int i = 0; i != world_building->door.size; i++) {
        current_door = my_vector_get(&world_building->door, i);
        building_delete(&world_building->all_building,
            &current_door->building_data);
        free(current_door);
    }
    my_vector_free(&world_building->door);
}