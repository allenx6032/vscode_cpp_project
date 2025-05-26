/*
** EPITECH PROJECT, 2021
** game
** File description:
** Game Calls
*/

#include "engine_rpg.h"
#include "engine_music.h"
#include "engine_camera.h"
#include "engine_sound.h"
#include "engine_font.h"
#include "engine_menus.h"
#include "engine_button.h"
#include <stdlib.h>

void game_init(game_t *game)
{
    window_init(&game->window, game->config);
    font_init(&game->fonts);
    music_init(&game->musics);
    sound_init(&game->sounds);
    camera_init(game);
    player_init(&game->player);
    menu_init(game);
    button_init(game);
    game->state = PLAYING_STATE;
    camera_render(game);
    game->has_been_started_once = false;
    menu_main_load(game);
}

void game_update(game_t *game)
{
    game_event(game);
    if (game->state == PLAYING_STATE) {
        map_update(game);
        player_update(&game->current_map, &game->player);
        camera_update(game);
        player_life_hud_update(&game->player, game->camera.view);
        player_camera_hud_update(&game->player.camera_hud, game);
    } else {
        if (game->state == PAUSE_STATE) {
            sfClock_restart(game->player.move_clock);
            sfClock_restart(game->camera.clock);
        }
    }
    menu_update(game);
    button_update(game);
}

void game_render(game_t *game)
{
    sfRenderWindow_clear(game->window, sfBlack);
    if (game->state == PLAYING_STATE || game->state == PAUSE_STATE ||
        game->state == INVENTORY_STATE) {
        map_render(game);
        player_render(game->window, game->player);
        camera_render(game);
        player_life_hud_render(game->window, game->player.life_hud);
        player_camera_hud_render(game->window, game->player.camera_hud);
    }
    menu_render(game);
    button_render(game);
    sfRenderWindow_display(game->window);
}

void game_destroy(game_t *game)
{
    window_destroy(&game->window);
    player_destroy(&game->player);
    if (game->has_been_started_once) {
        map_destroy(game);
        player_life_hud_destroy(&game->player.life_hud);
        player_camera_hud_destroy(&game->player.camera_hud);
    }
    menu_destroy(game);
    button_destroy(game);
    camera_destroy(game);
    music_destroy(&game->musics);
    sound_destroy(&game->sounds);
    font_destroy(&game->fonts);
}