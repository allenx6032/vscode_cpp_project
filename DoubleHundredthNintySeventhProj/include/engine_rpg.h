/*
** EPITECH PROJECT, 2021
** rpg
** File description:
** Header of MyRPG
*/

#ifndef RPG_H_
#define RPG_H_

#include <SFML_Graphics.h>
#include <stdbool.h>
#include "engine_vector.h"
#include "engine_player.h"
#include "engine_map.h"
#include "engine_menus.h"
#include "engine_camera.h"
#include "engine_config.h"

static const sfVideoMode VIDEO_MODE = { 1280, 720, 32 };

typedef enum game_state
{
    MAIN_MENU_STATE, PAUSE_STATE, PLAYING_STATE,
    INVENTORY_STATE, SETTINGS_STATE, HTP_STATE
} game_state;

typedef struct game_s
{
    sfRenderWindow *window;
    camera_t camera;
    player_t player;
    map_data_t current_map;
    sfEvent event;
    vector_t musics;
    vector_t sounds;
    vector_t fonts;
    menu_t menu;
    vector_t buttons;
    game_state state;
    config_t *config;
    bool has_been_started_once;
} game_t;

/*    GAME    */
void game_init(game_t *game);
void game_update(game_t *game);
void game_render(game_t *game);
void game_destroy(game_t *game);
void game_stop(game_t *game);
void game_start(game_t *game);

/*    GAME EVENT   */
void game_event(game_t *game);


/*    WINDOW    */
void window_init(sfRenderWindow **window, config_t *config);
void window_destroy(sfRenderWindow **window);

#endif /* !RPG_H_ */