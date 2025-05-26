/*
** EPITECH PROJECT, 2021
** player_camera_hud
** File description:
** Player Camera HUD
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_player.h"
#include "engine_rpg.h"
#include "engine_sfml_extra.h"

void player_camera_hud_init(player_camera_hud_t *camera_hud)
{
    camera_hud->sprite = sfSprite_create();
    camera_hud->texture = sfTexture_createFromFile(
        "./data/hud_camera_icon.png", NULL);
    sfTexture_setSmooth(camera_hud->texture, sfTrue);
    sfSprite_setTexture(camera_hud->sprite,
        camera_hud->texture, sfFalse);
    sfSprite_setScale(camera_hud->sprite, (sfVector2f){ 0.6f, 0.6f });
    player_camera_hud_init_asset(camera_hud);
    camera_hud->anim_clock = sfClock_create();
    camera_hud->zoom = false;
}

void player_camera_hud_set_image(player_camera_hud_t *camera_hud,
    game_t *game)
{
    if (game->camera.state != FREE_CAMERA)
        sfSprite_setTextureRect(camera_hud->sprite,
            (sfIntRect) { 0, 0, 88, 96 });
    else
        sfSprite_setTextureRect(camera_hud->sprite,
            (sfIntRect) { 88, 0, 88, 96 });
}

void player_camera_hud_update(player_camera_hud_t *camera_hud,
    game_t *game)
{
    const float top_x = sfView_getCenter(game->camera.view).x
        + (sfView_getSize(game->camera.view).x / 2);
    const float top_y = sfView_getCenter(game->camera.view).y
        - (sfView_getSize(game->camera.view).y / 2);
    sfVector2f mouse_pos =
        sfvector2f_fixmousepositionview(game->window, game->camera.view,
        (sfVector2f) { sfMouse_getPositionRenderWindow(game->window).x,
            sfMouse_getPositionRenderWindow(game->window).y });

    player_camera_hud_set_image(camera_hud, game);
    player_camera_hud_update_asset(camera_hud, game);
    if (sfsprite_isvectorinside(camera_hud->sprite, mouse_pos)
        || game->camera.state == FREE_CAMERA)
        player_camera_hud_animate(camera_hud, game);
    else
        sfSprite_setScale(camera_hud->sprite,
            (sfVector2f){ 0.6f, 0.6f });
    sfSprite_setPosition(camera_hud->sprite, (sfVector2f) {
        top_x - 5 - sfSprite_getGlobalBounds(camera_hud->sprite).width,
        top_y + 5
    });
}

void player_camera_hud_render(sfRenderWindow *window,
    player_camera_hud_t camera_hud)
{
    sfRenderWindow_drawSprite(window, camera_hud.sprite_back, NULL);
    sfRenderWindow_drawSprite(window, camera_hud.sprite, NULL);
    sfRenderWindow_drawSprite(window, camera_hud.sprite_back_rotate, NULL);
}

void player_camera_hud_destroy(player_camera_hud_t *camera_hud)
{
    sfSprite_destroy(camera_hud->sprite);
    sfSprite_destroy(camera_hud->sprite_back);
    sfSprite_destroy(camera_hud->sprite_back_rotate);
    sfTexture_destroy(camera_hud->texture);
    sfTexture_destroy(camera_hud->texture_back);
    sfTexture_destroy(camera_hud->texture_back_rotate);
    sfClock_destroy(camera_hud->anim_clock);
    sfClock_destroy(camera_hud->anim_clock_rotate);
}