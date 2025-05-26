/*
** EPITECH PROJECT, 2021
** building
** File description:
** Building for RPG
*/

#include "engine_building.h"

void building_init(world_building_t *building)
{
    my_vector_init(&building->all_building);
    tree_init(&building->tree);
    rock_init(&building->rock);
    mountains_init(&building->mountains);
    door_init(&building->door);
    key_init(&building->key);
}

void building_update(game_t *game, world_building_t *building)
{
    building_t *current_building;

    tree_update(building);
    rock_update(building);
    mountains_update(building);
    door_update(building);
    key_update(game, building);
    for (int i = 0; i != building->all_building.size; i++) {
        current_building = my_vector_get(&building->all_building, i);
        sfSprite_setTexture(current_building->sprite,
            current_building->texture, sfFalse);
        sfSprite_setPosition(current_building->sprite,
            current_building->position);
        sfSprite_setRotation(current_building->sprite,
            current_building->rotation);
    }
}

void building_render(sfRenderWindow *window, world_building_t *building)
{
    building_t *current_building;

    tree_render(window, building);
    rock_render(window, building);
    mountains_render(window, building);
    door_render(window, building);
    key_render(window, building);
    for (int i = 0; i != building->all_building.size; i++) {
        current_building = my_vector_get(&building->all_building, i);
        if (current_building->visible) {
            sfRenderWindow_drawSprite(window,
                current_building->sprite, NULL);
        }
    }
}

void building_destroy(world_building_t *building)
{
    building_t *current_building;

    tree_destroy(building);
    rock_destroy(building);
    mountains_destroy(building);
    door_destroy(building);
    key_destroy(building);
    for (int i = 0; i != building->all_building.size; i++) {
        current_building = my_vector_get(&building->all_building, i);
        sfSprite_destroy(current_building->sprite);
        sfTexture_destroy(current_building->texture);
    }
    my_vector_free(&building->all_building);
}