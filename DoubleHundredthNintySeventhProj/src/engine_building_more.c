/*
** EPITECH PROJECT, 2021
** building
** File description:
** Building More for RPG
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_building.h"

void building_create(vector_t *all_building, building_t *building,
    building_type type, sfVector2f pos)
{
    sfVector2f position_fixed = pos;

    building->type = type;
    building->sprite = sfSprite_create();
    building->texture = building_get_texture(type);
    building->visible = true;
    building->passable = true;
    building->rotation = 0;
    sfSprite_setTextureRect(building->sprite,
        building_get_default_texture_rec(type));
    sfSprite_setScale(building->sprite, building_get_size(type));
    sfSprite_setRotation(building->sprite, building->rotation);
    position_fixed.x -= sfSprite_getTextureRect(building->sprite).width / 2;
    position_fixed.y -= sfSprite_getTextureRect(building->sprite).height / 2;
    building->position = position_fixed;
    sfSprite_setPosition(building->sprite, building->position);
    my_vector_add(all_building, building);
}

void building_delete(vector_t *all_building, building_t *building)
{
    building_t *current_building;

    for (int i = 0; i != all_building->size; i++) {
        current_building = my_vector_get(all_building, i);
        if (current_building == building) {
            sfSprite_destroy(current_building->sprite);
            sfTexture_destroy(current_building->texture);
            my_vector_delete(all_building, i);
            return;
        }
    }
}

sfVector2f building_get_size(building_type type)
{
    if (type == DOOR_BUILDING)
        return (sfVector2f) { 0.5f, 0.5f };
    return (sfVector2f) { 1, 1 };
}

sfTexture *building_get_texture(building_type type)
{
    if (type == TREE_BUILDING)
        return sfTexture_createFromFile("./data/building.png", NULL);
    if (type == ROCK_BUILDING)
        return sfTexture_createFromFile("./data/building.png", NULL);
    if (type == MOUNTAINS_BUILDING)
        return sfTexture_createFromFile("./data/building.png", NULL);
    if (type == DOOR_BUILDING)
        return sfTexture_createFromFile("./data/building_more.png", NULL);
    if (type == KEY_BUILDING)
        return sfTexture_createFromFile("./data/building_more.png", NULL);
    return NULL;
}

sfIntRect building_get_default_texture_rec(building_type type)
{
    if (type == TREE_BUILDING)
        return (sfIntRect) { 96, 48, 48, 48 };
    if (type == ROCK_BUILDING)
        return (sfIntRect) { 192, 48, 48, 48 };
    if (type == MOUNTAINS_BUILDING)
        return (sfIntRect) { 0, 192, 96, 96 };
    if (type == DOOR_BUILDING)
        return (sfIntRect) { 1196, 182, 54, 60 };
    if (type == KEY_BUILDING)
        return (sfIntRect) { 1210, 6, 24, 24 };
    return (sfIntRect) { 0, 0, 0, 0 };
}