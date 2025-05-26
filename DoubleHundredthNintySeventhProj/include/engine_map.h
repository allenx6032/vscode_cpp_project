/*
** EPITECH PROJECT, 2021
** map
** File description:
** Header of Map for RPG
*/

#ifndef MAP_H_
#define MAP_H_

#include <SFML_Graphics.h>
#include <stdbool.h>
#include <stdlib.h>
#include "engine_vector.h"
#include "engine_building.h"
#include "engine_entity.h"
#include "engine_particule.h"

typedef struct game_s game_t;

typedef enum map_type
{
    TEST_MAP
} map_type;

typedef enum map_meteo_type
{
    RAIN_METEO, SUNNY_METEO, SNOW_METEO
} map_meteo_type;

typedef struct map_data_s
{
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f size;
    world_entity_t world_entity;
    world_building_t world_building;
    world_particule_t world_particule;
    map_type type;
    map_meteo_type meteo;
    char *name;
} map_data_t;

/*    MAP    */
void map_init(game_t *game);
void map_update(game_t *game);
void map_render(game_t *game);
void map_destroy(game_t *game);
bool map_sprite_can_move_at(map_data_t *current_map,
    sfSprite *moving_sprite, sfVector2f move_pos);

/*    MAP MORE    */
bool map_is_sprite_inside_entity(map_data_t *current_map,
    const sfSprite *sprite);
bool map_is_sprite_inside_building(map_data_t *current_map,
    const sfSprite *sprite);
int gen_random_nb(int from, int to);
void fill_map(game_t *game);

#endif /* !MAP_H_ */
