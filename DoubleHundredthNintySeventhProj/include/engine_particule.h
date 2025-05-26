/*
** EPITECH PROJECT, 2021
** particule
** File description:
** Particule Header for RPG
*/

#ifndef PARTICULE_H_
#define PARTICULE_H_

#include <SFML_Graphics.h>
#include <stdbool.h>
#include "engine_vector.h"

typedef struct game_s game_t;

typedef enum particule_type
{
    HEAL_PARTICULE
} particule_type;

typedef struct world_particule_s
{
    vector_t all_particule;
    vector_t heal;
} world_particule_t;

typedef struct particule_s
{
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f position;
    bool visible;
    bool loop;
    particule_type type;
    sfClock *anim_clock;
} particule_t;

typedef struct heal_s
{
    particule_t particule_data;
} heal_t;

/*    PARTICULE    */
void particule_init(world_particule_t *particule);
void particule_update(world_particule_t *particule);
void particule_render(sfRenderWindow *window, world_particule_t *particule);
void particule_destroy(world_particule_t *particule);

/*    PARTICULE MORE    */
void particule_create(vector_t *all_particule, particule_t *particule,
    particule_type type, sfVector2f pos);
void particule_delete(vector_t *all_particule, particule_t *particule);
sfTexture *particule_get_texture(particule_type type);
sfIntRect particule_get_default_texture_rec(particule_type type);

/*    HEAL    */
void heal_create(world_particule_t *particule, sfVector2f pos);
void heal_init(vector_t *heal_vec);
void heal_update(world_particule_t *particule);
void heal_render(sfRenderWindow *window, world_particule_t *particule);
void heal_destroy(world_particule_t *world_particule);

/*   HEAL MORE   */

void heal_delete(world_particule_t *particule, heal_t *heal);
void heal_animate(world_particule_t *particule, heal_t *heal);

#endif /* !PARTICULE_H_ */
