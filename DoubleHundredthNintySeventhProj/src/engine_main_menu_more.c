/*
** EPITECH PROJECT, 2021
** main_menu_more
** File description:
** Main Menu Loading
*/

#include "engine_rpg.h"
#include "engine_music.h"
#include "engine_teklibc.h"
#include "engine_button.h"

void menu_main_load_buttons(game_t *game)
{
    button_create(game, START_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y - 80 });
    button_create(game, HOW_TO_PLAY_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y - 20 });
    button_create(game, SETTINGS_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y + 40 });
    button_create(game, EXIT_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y + 100 });
}

void menu_main_load(game_t *game)
{
    if (game->state == SETTINGS_STATE) {
        game->menu.menu_main.parallax_pos =
            game->menu.menu_settings.parallax_pos;
        sfSprite_setTextureRect(game->menu.menu_main.parallax_sprite,
            sfSprite_getTextureRect(game->menu.menu_settings.parallax_sprite));
    }
    game->state = MAIN_MENU_STATE;
    music_stop_all(&game->musics);
    sound_stop_all(&game->sounds);
    music_play(&game->musics, CHILL_NOT_SAFE_MUSIC, sfTrue,
        (float) my_atoi(config_get(game->config, "volume")));
    button_destroy(game);
    button_init(game);
    menu_main_load_buttons(game);
}