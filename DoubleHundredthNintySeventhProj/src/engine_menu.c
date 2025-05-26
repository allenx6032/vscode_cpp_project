/*
** EPITECH PROJECT, 2021
** menu
** File description:
** All Menus of RPG
*/

#include "engine_rpg.h"

void menu_init(game_t *game)
{
    menu_pause_init(game);
    menu_main_init(game);
    menu_inventory_init(game);
    menu_settings_init(game);
    menu_htp_init(game);
}

void menu_update(game_t *game)
{
    if (game->state == PAUSE_STATE)
        menu_pause_update(game);
    if (game->state == MAIN_MENU_STATE)
        menu_main_update(game);
    if (game->state == INVENTORY_STATE)
        menu_inventory_update(game);
    if (game->state == SETTINGS_STATE)
        menu_settings_update(game);
    if (game->state == HTP_STATE)
        menu_htp_update(game);
}

void menu_render(game_t *game)
{
    if (game->state == PAUSE_STATE)
        menu_pause_render(game);
    if (game->state == MAIN_MENU_STATE)
        menu_main_render(game);
    if (game->state == INVENTORY_STATE)
        menu_inventory_render(game);
    if (game->state == SETTINGS_STATE)
        menu_settings_render(game);
    if (game->state == HTP_STATE)
        menu_htp_render(game);
}

void menu_destroy(game_t *game)
{
    menu_pause_destroy(game);
    menu_main_destroy(game);
    menu_inventory_destroy(game);
    menu_settings_destroy(game);
    menu_htp_destroy(game);
}