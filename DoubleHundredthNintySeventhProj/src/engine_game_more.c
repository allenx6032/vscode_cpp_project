/*
** EPITECH PROJECT, 2021
** game_more
** File description:
** game_more
*/

#include "engine_rpg.h"
#include "engine_button.h"
#include "engine_music.h"
#include "engine_sound.h"

void game_stop(game_t *game)
{
    music_stop_all(&game->musics);
    sound_stop_all(&game->sounds);
    menu_destroy(game);
    camera_destroy(game);
    button_destroy(game);
    player_destroy(&game->player);
    if (game->has_been_started_once) {
        map_destroy(game);
        player_life_hud_destroy(&game->player.life_hud);
        player_camera_hud_destroy(&game->player.camera_hud);
    }
}

void game_start(game_t *game)
{
    game_stop(game);
    map_init(game);
    camera_init(game);
    player_init(&game->player);
    menu_init(game);
    button_init(game);
    player_life_hud_init(&game->player.life_hud);
    player_camera_hud_init(&game->player.camera_hud);
    game->state = PLAYING_STATE;
    game->has_been_started_once = true;
}