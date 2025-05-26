/*
** EPITECH PROJECT, 2021
** entity
** File description:
** Header of Entity for RPG
*/

#ifndef ENTITY_H_
#define ENTITY_H_

#include <stdbool.h>
#include <SFML_Graphics.h>
#include <SFML_Clock.h>
#include "engine_vector.h"

typedef struct game_s game_t;

typedef enum entity_type
{
    MEDIC_ENTITY, ENEMY_ENTITY
} entity_type;

typedef struct world_entity_s
{
    vector_t all_entity;
    vector_t medic;
    vector_t enemy;
} world_entity_t;

typedef struct entity_s
{
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f position;
    int life;
    int damage;
    bool friendly;
    bool invulnerable;
    bool visible;
    entity_type type;
} entity_t;

typedef struct medic_s
{
    entity_t entity_data;
    sfClock *regen_clock;
} medic_t;

typedef struct enemy_s
{
    entity_t entity_data;
    unsigned int lvl;
    sfClock *damage_clock;
    sfClock *move_clock;
} enemy_t;

/*    ENTITY    */
void entity_init(world_entity_t *entity);
void entity_update(world_entity_t *entity, game_t *game);
void entity_render(sfRenderWindow *window, world_entity_t *entity);
void entity_destroy(world_entity_t *entity);
void entity_delete(vector_t *entity, entity_t *entity_data);
void damage_entity_arrond(world_entity_t *entity,
    sfVector2f pos, int dist, int damage);

/*    ENTITY_MORE    */
sfTexture *entity_get_texture(entity_type type);
sfIntRect entity_get_default_texture_rec(entity_type type);
void entity_create(vector_t *all_entity_vec, entity_t *entity_data,
    entity_type type, sfVector2f pos);

/*    MEDIC    */
void medic_create(world_entity_t *entity, sfVector2f pos);
void medic_init(vector_t *medic_vec);
void medic_update(world_entity_t *entity, game_t *game);
void medic_destroy(world_entity_t *entity);

/*    ENEMY   */
void enemy_create(world_entity_t *entity, sfVector2f pos);
void remove_enemy(world_entity_t *world_entity, entity_t entity);
void enemy_init(vector_t *enemy_vec);
void enemy_update(game_t *game, world_entity_t *entity);
void enemy_destroy(world_entity_t *world_entity);

#endif /* !ENTITY_H_ */