/*
** EPITECH PROJECT, 2021
** button_event
** File description:
** button_event
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"
#include "engine_button.h"

void button_action_settings(game_t *game, button_type type)
{
    int volume = my_atoi(config_get(game->config, "volume"));
    int fps = my_atoi(config_get(game->config, "fps"));
    int fullscreen = my_atoi(config_get(game->config, "fullscreen"));

    if (type == VOLUME_SETTINGS_BUTTON) {
        (volume >= 100) ? (volume = 0) : (volume += 5) ;
        config_set(game->config, "volume", my_itoa(volume));
    }
    if (type == FPS_SETTINGS_BUTTON) {
        (fps >= 120) ? (fps = 15) : (fps += 15);
        config_set(game->config, "fps", my_itoa(fps));
    }
    if (type == FULLSCREEN_SETTINGS_BUTTON) {
        (fullscreen >= 1) ? (fullscreen = 0) : (fullscreen = 1);
        config_set(game->config, "fullscreen", my_itoa(fullscreen));
    }
}

void button_action(game_t *game, button_type type)
{
    if (type == EXIT_BUTTON)
        sfRenderWindow_close(game->window);
    if (type == PAUSE_BUTTON)
        pause_game(game);
    if (type == MAIN_MENU_BUTTON)
        menu_main_load(game);
    if (type == START_BUTTON)
        game_start(game);
    if (type == SETTINGS_BUTTON)
        menu_settings_load(game);
    if (type == HOW_TO_PLAY_BUTTON)
        menu_htp_load(game);
    button_action_settings(game, type);
    sound_play(&game->sounds, BUTTON_ACTION_SOUND, sfFalse,
        (float) my_atoi(config_get(game->config, "volume")));
}