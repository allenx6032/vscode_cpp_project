/*
** EPITECH PROJECT, 2021
** htp_menu
** File description:
** How to play this bad and rushed game ?
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"
#include "engine_music.h"
#include "engine_button.h"

void menu_htp_load(game_t *game)
{
    game->state = HTP_STATE;
    music_stop_all(&game->musics);
    sound_stop_all(&game->sounds);
    music_play(&game->musics, ENEMY_THEME_MUSIC, sfTrue,
        (float) my_atoi(config_get(game->config, "volume")));
    button_destroy(game);
    button_init(game);
    button_create(game, MAIN_MENU_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x + 120,
        sfView_getCenter(game->camera.view).y + 74 });
    button_create(game, EXIT_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x + 120,
        sfView_getCenter(game->camera.view).y + 128 });
}

void menu_htp_init(game_t *game)
{
    game->menu.menu_htp.htp_sprite = sfSprite_create();
    game->menu.menu_htp.htp_texture =
        sfTexture_createFromFile("data/htp_bck.png", NULL);
    game->menu.menu_htp.htp_pos = (sfVector2f) {
        sfView_getCenter(game->camera.view).x
            - (sfView_getSize(game->camera.view).x / 2),
        sfView_getCenter(game->camera.view).y
            - (sfView_getSize(game->camera.view).y / 2)
    };
    sfSprite_setScale(game->menu.menu_htp.htp_sprite,
        (sfVector2f){ 0.501, 0.502 });
    sfSprite_setTexture(game->menu.menu_htp.htp_sprite,
        game->menu.menu_htp.htp_texture, sfFalse);
    sfSprite_setPosition(game->menu.menu_htp.htp_sprite,
        game->menu.menu_htp.htp_pos);
}

void menu_htp_update(game_t *game)
{
    game->menu.menu_htp.htp_pos = (sfVector2f) {
        sfView_getCenter(game->camera.view).x
            - (sfView_getSize(game->camera.view).x / 2),
        sfView_getCenter(game->camera.view).y
            - (sfView_getSize(game->camera.view).y / 2)
    };
    sfSprite_setPosition(game->menu.menu_htp.htp_sprite,
        game->menu.menu_htp.htp_pos);
}

void menu_htp_render(game_t *game)
{
    sfRenderWindow_drawSprite(game->window,
        game->menu.menu_htp.htp_sprite, NULL);
}

void menu_htp_destroy(game_t *game)
{
    sfSprite_destroy(game->menu.menu_htp.htp_sprite);
    sfTexture_destroy(game->menu.menu_htp.htp_texture);
}