/*
** EPITECH PROJECT, 2021
** player_camera_hud_more
** File description:
** player_camera_hud_more
*/

#include "engine_player.h"
#include "engine_sfml_extra.h"
#include "engine_teklibc.h"
#include "engine_rpg.h"

void player_camera_hud_click(game_t *game)
{
    if (game->camera.state != FREE_CAMERA) {
        game->camera.state = FREE_CAMERA;
        sound_play(&game->sounds, CAMERA_OUT_SOUND, sfFalse,
            (float) my_atoi(config_get(game->config, "volume")));
    } else {
        game->camera.state = PLAYING_CAMERA;
        sound_play(&game->sounds, CAMERA_IN_SOUND, sfFalse,
            (float) my_atoi(config_get(game->config, "volume")));
    }
}

void player_camera_hud_init_asset(player_camera_hud_t *camera_hud)
{
    camera_hud->sprite_back = sfSprite_create();
    camera_hud->sprite_back_rotate = sfSprite_create();
    camera_hud->texture_back = sfTexture_createFromFile(
        "./data/hud_camera_asset_1.png", NULL);
    camera_hud->texture_back_rotate = sfTexture_createFromFile(
        "./data/hud_camera_asset_2.png", NULL);
    sfTexture_setSmooth(camera_hud->texture_back, sfTrue);
    sfTexture_setSmooth(camera_hud->texture_back_rotate, sfTrue);
    sfSprite_setTexture(camera_hud->sprite_back,
        camera_hud->texture_back, sfFalse);
    sfSprite_setTexture(camera_hud->sprite_back_rotate,
        camera_hud->texture_back_rotate, sfFalse);
    sfSprite_setScale(camera_hud->sprite_back,
        (sfVector2f){ 0.6f, 0.6f });
    sfSprite_setScale(camera_hud->sprite_back_rotate,
        (sfVector2f){ 0.4f, 0.4f });
    camera_hud->anim_clock_rotate = sfClock_create();
}

void player_camera_hud_animate_asset(player_camera_hud_t *camera_hud,
    game_t *game)
{
    float current_rotation =
        sfSprite_getRotation(camera_hud->sprite_back_rotate);
    const float elapsed_time_sec = sfTime_asSeconds(
        sfClock_getElapsedTime(camera_hud->anim_clock_rotate));

    if (current_rotation < 360) {
        sfsprite_setrotationself(camera_hud->sprite_back_rotate,
            current_rotation + (100 * elapsed_time_sec));
    } else {
        sfsprite_setrotationself(camera_hud->sprite_back_rotate, 0);
    }
    sfClock_restart(camera_hud->anim_clock_rotate);
}

void player_camera_hud_update_asset(player_camera_hud_t *camera_hud,
    game_t *game)
{
    const float top_x = sfView_getCenter(game->camera.view).x
        + (sfView_getSize(game->camera.view).x / 2);
    const float top_y = sfView_getCenter(game->camera.view).y
        - (sfView_getSize(game->camera.view).y / 2);

    sfSprite_setPosition(camera_hud->sprite_back_rotate,
        (sfVector2f) { top_x - 10, top_y + 20});
    sfSprite_setPosition(camera_hud->sprite_back,
        (sfVector2f) { top_x - 80, top_y - 5});
    player_camera_hud_animate_asset(camera_hud, game);
}

void player_camera_hud_animate(player_camera_hud_t *camera_hud,
    game_t *game)
{
    sfVector2f scale = sfSprite_getScale(camera_hud->sprite);
    const float top_x = sfView_getCenter(game->camera.view).x
        + (sfView_getSize(game->camera.view).x / 2);
    const float top_y = sfView_getCenter(game->camera.view).y
        - (sfView_getSize(game->camera.view).y / 2);

    if (scale.x >= 0.63f || scale.y >= 0.63f)
            camera_hud->zoom = false;
    else if (scale.x <= 0.57f || scale.y <= 0.57f)
        camera_hud->zoom = true;
    if (camera_hud->zoom && sfTime_asSeconds(
        sfClock_getElapsedTime(camera_hud->anim_clock)) > 0.07f) {
        sfSprite_setScale(camera_hud->sprite,
            (sfVector2f){ scale.x + 0.01f, scale.y + 0.01f });
        sfClock_restart(camera_hud->anim_clock);
    } else if (!camera_hud->zoom && sfTime_asSeconds(
        sfClock_getElapsedTime(camera_hud->anim_clock)) > 0.07f) {
        sfSprite_setScale(camera_hud->sprite,
            (sfVector2f){ scale.x - 0.01f, scale.y - 0.01f });
        sfClock_restart(camera_hud->anim_clock);
    }
}