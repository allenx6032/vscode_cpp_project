/*
** EPITECH PROJECT, 2021
** menu
** File description:
** Menu Header for RPG
*/

#ifndef MENU_H_
#define MENU_H_

#include "engine_rpg.h"

typedef struct menu_inventory_s
{
    sfSprite *parallax_sprite;
    sfSprite *inv_sprite;
    sfTexture *parallax_texture;
    sfTexture *inv_texture;
    sfVector2f parallax_pos;
    sfVector2f inv_pos;
    sfText *title;
    sfSprite *item_map_sprite;
    sfSprite *item_potion_sprite;
    sfSprite *item_key_sprite;
} menu_inventory_t;

typedef struct menu_settings_s
{
    sfSprite *parallax_sprite;
    sfTexture *parallax_texture;
    sfVector2f parallax_pos;
    sfText *title;
} menu_settings_t;

typedef struct menu_main_s
{
    sfSprite *parallax_sprite;
    sfTexture *parallax_texture;
    sfVector2f parallax_pos;
    sfText *title;
} menu_main_t;

typedef struct menu_htp_s
{
    sfSprite *htp_sprite;
    sfTexture *htp_texture;
    sfVector2f htp_pos;
} menu_htp_t;

typedef struct menu_pause_s
{
    sfSprite *parallax_sprite;
    sfTexture *parallax_texture;
    sfVector2f parallax_pos;
    sfText *title;
} menu_pause_t;

typedef struct menu_s
{
    menu_pause_t menu_pause;
    menu_main_t menu_main;
    menu_settings_t menu_settings;
    menu_htp_t menu_htp;
    menu_inventory_t menu_inv;
} menu_t;

void menu_init(game_t *game);
void menu_update(game_t *game);
void menu_render(game_t *game);
void menu_destroy(game_t *game);

/* PAUSE */
void menu_pause_init(game_t *game);
void menu_pause_update(game_t *game);
void menu_pause_render(game_t *game);
void menu_pause_destroy(game_t *game);
void pause_game(game_t *game);

/* MAIN MENU */
void menu_main_init(game_t *game);
void menu_main_update(game_t *game);
void menu_main_render(game_t *game);
void menu_main_destroy(game_t *game);
void menu_main_load(game_t *game);

/* SETTINGS MENU */
void menu_settings_init(game_t *game);
void menu_settings_update(game_t *game);
void menu_settings_render(game_t *game);
void menu_settings_destroy(game_t *game);
void menu_settings_load(game_t *game);

/* HOW TO PLAY MENU */
void menu_htp_init(game_t *game);
void menu_htp_update(game_t *game);
void menu_htp_render(game_t *game);
void menu_htp_destroy(game_t *game);
void menu_htp_load(game_t *game);

/* INV MENU */
void menu_inventory_init(game_t *game);
void menu_inventory_update(game_t *game);
void menu_inventory_render(game_t *game);
void menu_inventory_destroy(game_t *game);
void inv_game(game_t *game);

void menu_inventory_update_parallax(game_t *game);
void menu_inventory_init_parallax(game_t *game);

#endif /* !MENU_H_ */
