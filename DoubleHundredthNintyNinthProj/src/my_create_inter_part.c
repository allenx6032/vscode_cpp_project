/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** create_inter_part
*/

#include "my_menu.h"

void create_menu_part_4_point_5(game_menu_t *game)
{
    sfFloatRect title_text_bounds = sfText_getGlobalBounds(
        game->sprites->title_text);
    sfVector2f title_text_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - title_text_bounds.width / 2, 150};
    sfFloatRect resume_text_bounds = sfText_getGlobalBounds(
        game->sprites->resume_text);
    sfVector2f resume_text_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - resume_text_bounds.width / 2, 150};

    sfText_setFillColor(game->sprites->resume_text, sfWhite);
    sfText_setPosition(game->sprites->resume_text, resume_text_position);
    sfText_setFillColor(game->sprites->title_text, sfWhite);
    sfText_setPosition(game->sprites->title_text, title_text_position);
}

void create_menu_part_3_point_5(game_menu_t *game)
{
    game->sprites->title_text = sfText_create();
    sfText_setFont(game->sprites->title_text, game->sprites->font);
    sfText_setString(game->sprites->title_text,
        "Catventure : a feline odyssey");
    sfText_setCharacterSize(game->sprites->title_text, 100);
    game->sprites->resume_text = sfText_create();
    sfText_setFont(game->sprites->resume_text, game->sprites->font);
    sfText_setString(game->sprites->resume_text,
        "You have a catventure to finish! Resume?");
    sfText_setCharacterSize(game->sprites->resume_text, 100);
}

void create_menu_part_1_point_5(game_menu_t *game)
{
    game->sprites->start_texture = sfTexture_createFromFile(
        "assets/menu/start.png", NULL);
    game->sprites->settings_texture = sfTexture_createFromFile(
        "assets/menu/settings.png", NULL);
    game->sprites->exit_texture = sfTexture_createFromFile(
        "assets/menu/exit.png", NULL);
}
