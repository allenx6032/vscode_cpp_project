/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** create menu
*/

#include "my_menu.h"

static void create_menu_part_5(game_menu_t *game)
{
    sfVector2f yes_text_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - 100, sfRenderWindow_getSize(
        game->video->window).y / 2 + 5};
    sfVector2f no_text_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - 100, sfRenderWindow_getSize(
        game->video->window).y / 2 + 205};

    game->sprites->yes_text = sfText_create();
    sfText_setFont(game->sprites->yes_text, game->sprites->font);
    sfText_setString(game->sprites->yes_text, "YES");
    sfText_setCharacterSize(game->sprites->yes_text, 100);
    sfText_setFillColor(game->sprites->yes_text, sfWhite);
    sfText_setPosition(game->sprites->yes_text, yes_text_position);
    game->sprites->no_text = sfText_create();
    sfText_setFont(game->sprites->no_text, game->sprites->font);
    sfText_setString(game->sprites->no_text, "NO!");
    sfText_setCharacterSize(game->sprites->no_text, 100);
    sfText_setFillColor(game->sprites->no_text, sfWhite);
    sfText_setPosition(game->sprites->no_text, no_text_position);
}

static void create_menu_part_4(game_menu_t *game)
{
    sfVector2f no_size = {400, 150};
    sfVector2f no_position = {sfRenderWindow_getSize(game->video->window).x / 2
        - no_size.x / 2 - 30,
        sfRenderWindow_getSize(game->video->window).y / 2 + 200};

    game->sprites->no_button = sfRectangleShape_create();
    sfRectangleShape_setSize(game->sprites->no_button, no_size);
    sfRectangleShape_setPosition(game->sprites->no_button, no_position);
    sfRectangleShape_setFillColor(game->sprites->no_button, sfRed);
    create_menu_part_4_point_5(game);
    create_menu_part_5(game);
}

static void create_menu_part_3(game_menu_t *game)
{
    sfVector2f yes_size = {400, 150};
    sfVector2f yes_position = {sfRenderWindow_getSize(game->video->window).x / 2
        - yes_size.x / 2 - 30,
        sfRenderWindow_getSize(game->video->window).y / 2};

    game->sprites->yes_button = sfRectangleShape_create();
    sfRectangleShape_setSize(game->sprites->yes_button, yes_size);
    sfRectangleShape_setPosition(game->sprites->yes_button, yes_position);
    sfRectangleShape_setFillColor(game->sprites->yes_button, sfGreen);
    game->sprites->font = sfFont_createFromFile(
        "assets/menu/Stardew_Valley.ttf");
    create_menu_part_3_point_5(game);
    create_menu_part_4(game);
}

static void create_menu_part_2(game_menu_t *game)
{
    sfVector2f settings_size = {200, 50};
    sfVector2f settings_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - settings_size.x + 10,
        sfRenderWindow_getSize(game->video->window).y / 2 + 150};
    sfVector2f exit_size = {200, 50};
    sfVector2f exit_position = {sfRenderWindow_getSize(game->video->window).x /
        2 - exit_size.x + 10,
        sfRenderWindow_getSize(game->video->window).y / 2 + 350};

    game->sprites->settings_button = sfSprite_create();
    sfSprite_setTexture(game->sprites->settings_button,
        game->sprites->settings_texture, sfTrue);
    sfSprite_setPosition(game->sprites->settings_button, settings_position);
    sfSprite_setScale(game->sprites->settings_button, (sfVector2f){0.5, 0.5});
    game->sprites->exit_button = sfSprite_create();
    sfSprite_setTexture(game->sprites->exit_button, game->sprites->exit_texture,
        sfTrue);
    sfSprite_setPosition(game->sprites->exit_button, exit_position);
    sfSprite_setScale(game->sprites->exit_button, (sfVector2f){0.5, 0.5});
    create_menu_part_3(game);
}

void create_menu_part_1(game_menu_t *game)
{
    sfVector2f button_size = {200, 50};
    sfVector2f button_position = {sfRenderWindow_getSize(
        game->video->window).x / 2 - button_size.x + 10, sfRenderWindow_getSize(
        game->video->window).y / 2 - button_size.y};

    game->sprites->backgroundTexture = sfTexture_createFromFile(
        "assets/menu/background_menu.png", NULL);
    game->sprites->backgroundSprite = sfSprite_create();
    sfSprite_setTexture(game->sprites->backgroundSprite,
        game->sprites->backgroundTexture, sfTrue);
    create_menu_part_1_point_5(game);
    game->sprites->start_button = sfSprite_create();
    sfSprite_setTexture(game->sprites->start_button,
        game->sprites->start_texture, sfTrue);
    sfSprite_setPosition(game->sprites->start_button, button_position);
    sfSprite_setScale(game->sprites->start_button, (sfVector2f) {0.5, 0.5});
    create_menu_part_2(game);
}
