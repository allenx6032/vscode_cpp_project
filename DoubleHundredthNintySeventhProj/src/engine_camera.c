/*
** EPITECH PROJECT, 2021
** camera
** File description:
** Camera basic game calls
*/

#include "engine_rpg.h"
#include "engine_sfml_extra.h"

void camera_init(game_t *game)
{
    game->camera.view = sfView_create();
    sfView_setSize(game->camera.view, (sfVector2f){
        640,
        360
    });
    game->camera.state = PLAYING_CAMERA;
    game->camera.target_pos = (sfVector2f) { 0, 0 };
    game->camera.clock = sfClock_create();
}

void camera_listen_free_move(game_t *game)
{
    const float elapsed_time_sec =
        sfTime_asSeconds(sfClock_getElapsedTime(game->camera.clock));

    if (game->camera.state == FREE_CAMERA) {
        if (sfKeyboard_isKeyPressed(sfKeyDown))
            game->camera.target_pos.y += elapsed_time_sec * 200;
        if (sfKeyboard_isKeyPressed(sfKeyLeft))
            game->camera.target_pos.x -= elapsed_time_sec * 200;
        if (sfKeyboard_isKeyPressed(sfKeyRight))
            game->camera.target_pos.x += elapsed_time_sec * 200;
        if (sfKeyboard_isKeyPressed(sfKeyUp))
            game->camera.target_pos.y -= elapsed_time_sec * 200;
        sfClock_restart(game->camera.clock);
    }
}

void camera_update(game_t *game)
{
    sfVector2f pos;

    if (game->camera.state == PLAYING_CAMERA) {
        pos = game->player.position;
        pos.x += sfSprite_getTextureRect(game->player.sprite).width / 2;
        pos.y += sfSprite_getTextureRect(game->player.sprite).height / 2;
        game->camera.target_pos = pos;
    }
    if (game->camera.state == TRANSITION_CAMERA) {
        camera_transition_update(game);
        return;
    } else if (game->camera.state == FREE_CAMERA)
        camera_listen_free_move(game);
    if (game->camera.state == WAIT_CAMERA)
        if (sfTime_asMilliseconds(sfClock_getElapsedTime(
            game->camera.clock)) >= game->camera.wait_ms) {
            game->camera.state = PLAYING_CAMERA;
            sfClock_restart(game->camera.clock);
        }
    sfView_setCenter(game->camera.view, game->camera.target_pos);
}

void camera_render(game_t *game)
{
    sfRenderWindow_setView(game->window, game->camera.view);
}

void camera_destroy(game_t *game)
{
    sfView_destroy(game->camera.view);
    sfClock_destroy(game->camera.clock);
}