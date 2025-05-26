/*
** EPITECH PROJECT, 2021
** pause_more
** File description:
** Pause More for RPG
*/

#include "engine_rpg.h"
#include "engine_button.h"
#include "engine_teklibc.h"
#include "engine_music.h"
#include "engine_sound.h"

void pause_game_load(game_t *game)
{
    game->state = PAUSE_STATE;
    sound_pause_all(&game->sounds);
    music_pause_all(&game->musics);
    music_play(&game->musics, PAUSE_MUSIC, sfTrue,
        (float) my_atoi(config_get(game->config, "volume")));
    button_create(game, PAUSE_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y - 80 });
    button_create(game, MAIN_MENU_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y - 20 });
    button_create(game, EXIT_BUTTON, (sfVector2f)(sfVector2f) {
        sfView_getCenter(game->camera.view).x - 95,
        sfView_getCenter(game->camera.view).y + 40 });
}

void pause_game_unload(game_t *game)
{
    game->state = PLAYING_STATE;
    music_stop(&game->musics, PAUSE_MUSIC);
    sound_resume_all(&game->sounds);
    music_resume_all(&game->musics);
    button_destroy(game);
    button_init(game);
}

void pause_game(game_t *game)
{
    if (game->state == PLAYING_STATE) {
        pause_game_load(game);
    } else if (game->state == PAUSE_STATE) {
        pause_game_unload(game);
    }
}