/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** resize
*/

#include "my_menu.h"

static void resize_prt_2(game_menu_t *game, sfEvent *event)
{
    sfVector2f win_size = sfView_getSize(sfRenderWindow_getView(
        game->video->window));
    sfVector2f start = sfSprite_getPosition(game->sprites->start_button);
    sfVector2f settings = sfSprite_getPosition(game->sprites->settings_button);
    sfVector2f exit = sfSprite_getPosition(game->sprites->exit_button);
    sfVector2f title = sfText_getPosition(game->sprites->title_text);

    sfSprite_setPosition(game->sprites->start_button, (sfVector2f) {start.x *
        event->size.width / win_size.x, start.y * event->size.height /
        win_size.y});
    sfSprite_setPosition(game->sprites->settings_button, (sfVector2f)
        {settings.x * event->size.width / win_size.x, settings.y *
        event->size.height / win_size.y});
    sfSprite_setPosition(game->sprites->exit_button, (sfVector2f) {exit.x *
        event->size.width / win_size.x, exit.y * event->size.height /
        win_size.y});
    sfText_setPosition(game->sprites->title_text, (sfVector2f) {title.x *
        event->size.width / win_size.x, title.y * event->size.height /
        win_size.y});
}

void resize_menu(game_menu_t *game, sfEvent *event)
{
    sfVector2f win_size = sfView_getSize(sfRenderWindow_getView(
        game->video->window));
    sfFloatRect visibleArea = {0.f, 0.f, event->size.width, event->size.height};

    sfRenderWindow_setView(game->video->window,
        sfView_createFromRect(visibleArea));
    sfSprite_scale(game->sprites->backgroundSprite, (sfVector2f)
        {event->size.width / win_size.x, event->size.height / win_size.y});
    sfSprite_scale(game->sprites->start_button, (sfVector2f) {event->size.width
        / win_size.x, event->size.height / win_size.y});
    sfSprite_scale(game->sprites->settings_button, (sfVector2f)
        {event->size.width / win_size.x, event->size.height / win_size.y});
    sfSprite_scale(game->sprites->exit_button, (sfVector2f) {event->size.width
        / win_size.x, event->size.height / win_size.y});
    sfText_scale(game->sprites->title_text, (sfVector2f) {event->size.width
        / win_size.x, event->size.height / win_size.y});
    resize_prt_2(game, event);
}
