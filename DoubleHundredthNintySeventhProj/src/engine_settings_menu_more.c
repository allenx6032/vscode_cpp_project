/*
** EPITECH PROJECT, 2021
** main_menu_more
** File description:
** Settings Menu Load
*/

#include "engine_rpg.h"
#include "engine_music.h"
#include "engine_teklibc.h"
#include "engine_button.h"

void menu_settings_load_buttons(game_t *game)
{
    button_create(game, MAIN_MENU_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 280,
        sfView_getCenter(game->camera.view).y + 110 });
    button_create(game, EXIT_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x + 90,
        sfView_getCenter(game->camera.view).y + 110 });
    button_create(game, FPS_SETTINGS_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 260,
        sfView_getCenter(game->camera.view).y - 60 });
    button_create(game, VOLUME_SETTINGS_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x + 90,
        sfView_getCenter(game->camera.view).y - 60 });
    button_create(game, FULLSCREEN_SETTINGS_BUTTON, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 85,
        sfView_getCenter(game->camera.view).y + 10 });
}

void menu_settings_load(game_t *game)
{
    if (game->state == MAIN_MENU_STATE) {
        game->menu.menu_settings.parallax_pos =
            game->menu.menu_main.parallax_pos;
        sfSprite_setTextureRect(game->menu.menu_main.parallax_sprite,
            sfSprite_getTextureRect(game->menu.menu_main.parallax_sprite));
    }
    game->state = SETTINGS_STATE;
    music_stop_all(&game->musics);
    sound_stop_all(&game->sounds);
    music_play(&game->musics, ENEMY_THEME_MUSIC, sfTrue,
        (float) my_atoi(config_get(game->config, "volume")));
    button_destroy(game);
    button_init(game);
    menu_settings_load_buttons(game);
}