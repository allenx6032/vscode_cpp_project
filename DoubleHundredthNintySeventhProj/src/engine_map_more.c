/*
** EPITECH PROJECT, 2021
** map_more
** File description:
** More Map for RPG
*/

#include "engine_rpg.h"
#include "engine_sfml_extra.h"

bool map_is_sprite_inside_entity(map_data_t *current_map,
    const sfSprite *sprite)
{
    entity_t *current_entity;

    for (int i = 0; i != current_map->world_entity.all_entity.size; i++) {
        current_entity = my_vector_get(
            &current_map->world_entity.all_entity, i);
        if (sfsprite_isspriteinside(current_entity->sprite, sprite))
            return true;
    }
    return false;
}

bool map_is_sprite_inside_building(map_data_t *current_map,
    const sfSprite *sprite)
{
    building_t *current_building;

    for (int i = 0; i != current_map->world_building.all_building.size; i++) {
        current_building = my_vector_get(
            &current_map->world_building.all_building, i);
        if (!current_building->passable) {
            if (sfsprite_isspriteinside(current_building->sprite,
                sprite))
                return true;
        }
    }
    return false;
}

int gen_random_nb(int from, int to)
{
    return (rand() % to) + from;
}

void fill_map(game_t *game)
{
    medic_create(&game->current_map.world_entity, (sfVector2f) { 820, 720 });
    enemy_create(&game->current_map.world_entity, (sfVector2f) { 890, 720 });
    for (int i = 0; i != 10; i += 1)
        tree_create(&game->current_map.world_building, (sfVector2f) {
            gen_random_nb(20, 1200), gen_random_nb(20, 1200) });
    for (int i = 0; i != 5; i += 1)
        rock_create(&game->current_map.world_building, (sfVector2f) {
            gen_random_nb(20, 1200), gen_random_nb(20, 1200) });
    mountains_create(&game->current_map.world_building,
        (sfVector2f) { 300, 480 });
    key_create(&game->current_map.world_building, (sfVector2f) { 300, 340 });
}