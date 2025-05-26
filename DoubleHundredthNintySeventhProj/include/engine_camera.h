/*
** EPITECH PROJECT, 2021
** camera
** File description:
** Header of Camera for RPG
*/

#ifndef CAMERA_H_
#define CAMERA_H_

#include "engine_sound.h"

typedef struct game_s game_t;

typedef enum camera_state
{
    FREE_CAMERA, WAIT_CAMERA,
    PLAYING_CAMERA, TRANSITION_CAMERA
} camera_state;

typedef struct camera_s
{
    sfView *view;
    sfVector2f target_pos;
    camera_state state;
    sfClock *clock;
    float wait_ms;
    sound_type type;
} camera_t;

void camera_init(game_t *game);
void camera_update(game_t *game);
void camera_render(game_t *game);
void camera_destroy(game_t *game);
void camera_transition_to(camera_t *camera, sfVector2f pos,
    sound_type type, float wait_ms);

/*    CAMERA MORE     */
void camera_transition_update(game_t *game);

#endif /* !CAMERA_H_ */
