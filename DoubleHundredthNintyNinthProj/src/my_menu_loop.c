/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** menu_loop
*/

#include <SFML_Graphics.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "my_menu.h"
#include "my_typedef.h"
#include "my_utils.h"
#include "my_create.h"

void is_hover(sfEvent *event, game_menu_t *game)
{
    if (event->type == sfEvtMouseMoved) {
        game->video->yes_bounds =
            sfRectangleShape_getGlobalBounds(game->sprites->yes_button);
        game->video->no_bounds = sfRectangleShape_getGlobalBounds(
        game->sprites->no_button);
        if (sfFloatRect_contains(&game->video->yes_bounds,
            event->mouseMove.x, event->mouseMove.y)) {
                sfRectangleShape_setFillColor(
                game->sprites->yes_button, sfColor_fromRGB(150, 255, 150));
        } else
            sfRectangleShape_setFillColor(game->sprites->yes_button, sfGreen);
        if (sfFloatRect_contains(&game->video->no_bounds,
            event->mouseMove.x, event->mouseMove.y)) {
            sfRectangleShape_setFillColor(game->sprites->no_button,
            sfColor_fromRGB(255, 150, 150));
        } else
            sfRectangleShape_setFillColor(game->sprites->no_button, sfRed);
    }
}

bool main_loop_cond_bis(rpg_t *rpg)
{
    if (rpg->character->perso != NULL) {
        sfSprite_setPosition(rpg->character->sprite, (sfVector2f)
            {rpg->character->perso->pos_x, rpg->character->perso->pos_y});
        sfSprite_setPosition(rpg->menu->map->sprite
            [rpg->character->perso->id_zone], (sfVector2f)
            {rpg->character->perso->map_pos_x,
            rpg->character->perso->map_pos_y});
        sfSprite_setPosition(rpg->menu->map->sprite
            [rpg->character->perso->id_zone + 1], (sfVector2f)
            {rpg->character->perso->map_pos_x,
            rpg->character->perso->map_pos_y});
        return false;
    } else {
        rpg->character->perso = create_perso(false, rpg->env);
        return true;
    }
}

bool main_loop_cond_is_in_bounds(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    if (sfFloatRect_contains(&game->video->yes_bounds, event->mouseButton.x,
        event->mouseButton.y)) {
        sfSound_play(game->audio->meow_click);
        game->start_button_pressed = false;
        sfMusic_stop(game->audio->menu_song);
        rpg->character->perso = create_perso(true, rpg->env);
        return main_loop_cond_bis(rpg);
    } else if (sfFloatRect_contains(&game->video->no_bounds,
        event->mouseButton.x, event->mouseButton.y)) {
        sfSound_play(game->audio->meow_click);
        game->start_button_pressed = false;
        sfMusic_stop(game->audio->menu_song);
        rpg->character->perso = create_perso(false, rpg->env);
        return true;
    }
    return true;
}

bool main_loop_cond(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    if (event->mouseButton.button == sfMouseLeft) {
        game->video->yes_bounds = sfRectangleShape_getGlobalBounds(
            game->sprites->yes_button);
        game->video->no_bounds = sfRectangleShape_getGlobalBounds(
        game->sprites->no_button);
        return main_loop_cond_is_in_bounds(game, event, rpg);
    }
    return false;
}

bool main_loop(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    bool test = false;

    while (sfRenderWindow_pollEvent(
        game->video->window, event)) {
        if (event->type == sfEvtClosed) {
            sfRenderWindow_close(game->video->window);
        }
        is_hover(event, game);
        is_escaped(event, game);
        if (event->type == sfEvtMouseButtonPressed) {
            test = main_loop_cond(game, event, rpg);
        }
    }
    return test;
}
