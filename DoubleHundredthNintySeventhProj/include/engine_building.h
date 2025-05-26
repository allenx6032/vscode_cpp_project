/*
** EPITECH PROJECT, 2021
** building
** File description:
** Header of Building for RPG
*/

#ifndef BUILDING_H_
#define BUILDING_H_

#include <SFML_Graphics.h>
#include <stdbool.h>
#include "engine_vector.h"

typedef struct game_s game_t;

typedef enum building_type
{
    TREE_BUILDING, ROCK_BUILDING, MOUNTAINS_BUILDING,
    DOOR_BUILDING, KEY_BUILDING
} building_type;

typedef enum door_state
{
    DOOR_OPEN, DOOR_CLOSED
} door_state;

typedef struct world_building_s
{
    vector_t all_building;
    vector_t tree;
    vector_t rock;
    vector_t mountains;
    vector_t door;
    vector_t key;
} world_building_t;

typedef struct building_s
{
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f position;
    bool passable;
    bool visible;
    float rotation;
    building_type type;
} building_t;

typedef struct tree_s
{
    building_t building_data;
} tree_t;

typedef struct rock_s
{
    building_t building_data;
} rock_t;

typedef struct mountains_s
{
    building_t building_data;
} mountains_t;

typedef struct key_b_s
{
    building_t building_data;
} key_b_t;

typedef struct door_s
{
    building_t building_data;
    sfVector2f teleport_to_pos;
    door_state state;
    unsigned char map_type;
} door_t;

void building_init(world_building_t *building);
void building_update(game_t *game, world_building_t *building);
void building_render(sfRenderWindow *window, world_building_t *building);
void building_destroy(world_building_t *building);

void building_create(vector_t *all_building, building_t *building,
    building_type type, sfVector2f pos);
void building_delete(vector_t *all_building, building_t *building);
sfVector2f building_get_size(building_type type);
sfTexture *building_get_texture(building_type type);
sfIntRect building_get_default_texture_rec(building_type type);

void tree_create(world_building_t *building, sfVector2f pos);
void tree_init(vector_t *tree_vec);
void tree_update(world_building_t *building);
void tree_render(sfRenderWindow *window, world_building_t *building);
void tree_destroy(world_building_t *world_building);

void rock_create(world_building_t *building, sfVector2f pos);
void rock_init(vector_t *rock_vec);
void rock_update(world_building_t *building);
void rock_render(sfRenderWindow *window, world_building_t *building);
void rock_destroy(world_building_t *world_building);

void mountains_create(world_building_t *building, sfVector2f pos);
void mountains_init(vector_t *mountains_vec);
void mountains_update(world_building_t *building);
void mountains_render(sfRenderWindow *window, world_building_t *building);
void mountains_destroy(world_building_t *world_building);

void door_create(world_building_t *building, sfVector2f pos);
void door_init(vector_t *door_vec);
void door_update(world_building_t *building);
void door_render(sfRenderWindow *window, world_building_t *building);
void door_destroy(world_building_t *world_building);

void key_create(world_building_t *building, sfVector2f pos);
void key_init(vector_t *key_vec);
void key_update(game_t *game, world_building_t *building);
void key_render(sfRenderWindow *window, world_building_t *building);
void key_destroy(world_building_t *world_building);
void key_update_config(game_t *game, key_b_t *current_key);

#endif /* !BUILDING_H_ */