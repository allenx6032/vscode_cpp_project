/*
** EPITECH PROJECT, 2021
** camera_more
** File description:
** Camera game calls
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"
#include "engine_sfml_extra.h"

void camera_transition_to(camera_t *camera, sfVector2f pos,
    sound_type type, float wait_ms)
{
    camera->state = TRANSITION_CAMERA;
    camera->target_pos = pos;
    camera->type = type;
    camera->wait_ms = wait_ms;
    camera->clock = sfClock_create();
}

sfVector2f camera_get_transition_vec(const sfVector2f current_pos,
    const sfVector2f wanted_pos, sfClock *clock)
{
    sfVector2f vec_diff = sfvector2f_diff(current_pos, wanted_pos);
    sfVector2f final_vec = (sfVector2f) { 0, 0 };
    const float elapsed_time_sec =
        sfTime_asSeconds(sfClock_getElapsedTime(clock));

    if (vec_diff.x != 0.0f)
        final_vec.x = vec_diff.x > 0.0f ? elapsed_time_sec * -100.0f:
            elapsed_time_sec * 100.0f;
    if (vec_diff.x  < 0 && vec_diff.x > (elapsed_time_sec * -100.0f))
        final_vec.x = vec_diff.x * -1;
    if (vec_diff.x  > 0 && vec_diff.x < (elapsed_time_sec * 100.0f))
        final_vec.x = vec_diff.x * -1;
    if (vec_diff.y != 0.0f)
        final_vec.y = vec_diff.y > 0.0f ? elapsed_time_sec * -100.0f:
            elapsed_time_sec * 100.0f;
    if (vec_diff.y  < 0 && vec_diff.y > elapsed_time_sec * -100.0f)
        final_vec.y = vec_diff.y * -1;
    if (vec_diff.y  > 0 && vec_diff.y < elapsed_time_sec * 100.0f)
        final_vec.y = vec_diff.y * -1;
    return final_vec;
}

void camera_do_transition(camera_t *camera)
{
    sfVector2f pos = sfView_getCenter(camera->view);
    sfVector2f transition_pos = camera_get_transition_vec(pos,
        camera->target_pos, camera->clock);

    pos.x += transition_pos.x;
    pos.y += transition_pos.y;
    sfView_setCenter(camera->view, pos);
}

void camera_transition_update(game_t *game)
{
    if (sfvector2f_isdifferent(sfView_getCenter(game->camera.view),
        game->camera.target_pos)) {
        camera_do_transition(&game->camera);
        sfClock_restart(game->camera.clock);
    } else {
        sound_play(&game->sounds, game->camera.type, sfFalse,
            (float) my_atoi(config_get(game->config, "volume")));
        game->camera.state = WAIT_CAMERA;
        sfClock_restart(game->camera.clock);
    }
}