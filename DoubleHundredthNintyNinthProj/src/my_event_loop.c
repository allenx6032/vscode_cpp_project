/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** event_loop
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

bool event_cond_loop(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    bool test = false;

    while (game->start_button_pressed == true &&
        sfRenderWindow_isOpen(game->video->window)) {
        sfRenderWindow_clear(game->video->window,
        sfColor_fromRGB(125, 50, 200));
        sfRenderWindow_drawText(game->video->window,
        game->sprites->resume_text, NULL);
        sfRenderWindow_drawRectangleShape(
            game->video->window, game->sprites->yes_button, NULL);
        sfRenderWindow_drawRectangleShape(
            game->video->window, game->sprites->no_button, NULL);
        sfRenderWindow_drawText(game->video->window,
        game->sprites->yes_text, NULL);
        sfRenderWindow_drawText(game->video->window,
        game->sprites->no_text, NULL);
        sfRenderWindow_display(game->video->window);
        test = main_loop(game, event, rpg) || test;
    }
    return test;
}

bool main_event_cond(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    if (sfFloatRect_contains(&game->video->start_bounds, event->mouseButton.x,
        event->mouseButton.y)) {
        sfSound_play(game->audio->menu_click);
        game->start_button_pressed = true;
        return event_cond_loop(game, event, rpg);
    }
    if (sfFloatRect_contains(&game->video->settings_bounds,
        event->mouseButton.x, event->mouseButton.y)) {
        sfSound_play(game->audio->menu_click);
    } else if (sfFloatRect_contains(&game->video->exit_bounds,
        event->mouseButton.x, event->mouseButton.y)) {
        sfSound_play(game->audio->menu_click);
        sfRenderWindow_close(game->video->window);
    }
    return false;
}

static bool event_loop_bis(game_menu_t *game, sfEvent *event, rpg_t *rpg,
bool test)
{
    if (event->type == sfEvtMouseButtonPressed &&
        event->mouseButton.button == sfMouseLeft) {
        game->video->start_bounds = sfSprite_getGlobalBounds(
            game->sprites->start_button);
        game->video->settings_bounds = sfSprite_getGlobalBounds(
            game->sprites->settings_button);
        game->video->exit_bounds = sfSprite_getGlobalBounds(
            game->sprites->exit_button);
        test = main_event_cond(game, event, rpg);
    }
    return test;
}

bool event_loop(game_menu_t *game, sfEvent *event, rpg_t *rpg)
{
    bool test = false;

    if (event->type == sfEvtClosed)
        sfRenderWindow_close(game->video->window);
    if (event->type == sfEvtResized)
        resize_menu(game, event);
    if (event->type == sfEvtMouseMoved) {
        game->video->start_bounds = sfSprite_getGlobalBounds(
            game->sprites->start_button);
        game->video->settings_bounds = sfSprite_getGlobalBounds(
            game->sprites->settings_button);
        game->video->exit_bounds = sfSprite_getGlobalBounds(
            game->sprites->exit_button);
        start_bounds_cond(game, event);
        settings_bounds_cond(game, event);
        exit_bounds_cond(game, event);
    }
    test = event_loop_bis(game, event, rpg, test);
    return test;
}
