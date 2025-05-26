/*
** EPITECH PROJECT, 2021
** inventory_menu
** File description:
** Inventory Menu for RPG
*/

#include <stdlib.h>
#include "engine_rpg.h"
#include "engine_music.h"
#include "engine_font.h"
#include "engine_button.h"

void menu_inventory_init_items(game_t *game)
{
    menu_inventory_t *menu_inv = &game->menu.menu_inv;

    menu_inv->item_map_sprite = sfSprite_create();
    sfSprite_setTexture(menu_inv->item_map_sprite,
        game->player.items.map_texture, sfFalse);
    sfSprite_setTextureRect(menu_inv->item_map_sprite,
        game->player.items.map_rect);
    sfSprite_setScale(menu_inv->item_map_sprite, (sfVector2f) { 0.3, 0.3 });
    menu_inv->item_potion_sprite = sfSprite_create();
    sfSprite_setTexture(menu_inv->item_potion_sprite,
        game->player.items.potion_texture, sfFalse);
    sfSprite_setTextureRect(menu_inv->item_potion_sprite,
        game->player.items.potion_rect);
    sfSprite_setScale(menu_inv->item_potion_sprite, (sfVector2f) { 0.7, 0.7 });
    menu_inv->item_key_sprite = sfSprite_create();
    sfSprite_setTexture(menu_inv->item_key_sprite,
        game->player.items.key_texture, sfFalse);
    sfSprite_setTextureRect(menu_inv->item_key_sprite,
        game->player.items.key_rect);
    sfSprite_setScale(menu_inv->item_key_sprite, (sfVector2f) { 1.1, 1.1 });
}

void menu_inventory_init(game_t *game)
{
    menu_inventory_t *menu_inv = &game->menu.menu_inv;

    menu_inventory_init_parallax(game);
    menu_inventory_init_items(game);
    menu_inv->inv_sprite = sfSprite_create();
    menu_inv->inv_texture = sfTexture_createFromFile("./data/inv.png", NULL);
    menu_inv->inv_pos = (sfVector2f) {
        sfView_getCenter(game->camera.view).x,
        sfView_getCenter(game->camera.view).y };
    sfSprite_setTexture(menu_inv->inv_sprite, menu_inv->inv_texture, sfFalse);
    sfSprite_setPosition(menu_inv->inv_sprite, menu_inv->inv_pos);
    menu_inv->title = sfText_create();
    sfText_setString(menu_inv->title, "Inventory");
    sfText_setFont(menu_inv->title,
        font_get(&game->fonts, ANCIENT_MEDIUM_FONT)->font);
    sfText_setCharacterSize(menu_inv->title, 45);
}

void menu_inventory_update(game_t *game)
{
    menu_inventory_update_parallax(game);
    sfText_setPosition(game->menu.menu_inv.title, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 85,
        sfView_getCenter(game->camera.view).y - 140 });
    game->menu.menu_inv.inv_pos = (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 185,
        sfView_getCenter(game->camera.view).y - 50};
    sfSprite_setPosition(game->menu.menu_inv.inv_sprite,
        game->menu.menu_inv.inv_pos);
    sfSprite_setPosition(game->menu.menu_inv.item_map_sprite, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 145,
        sfView_getCenter(game->camera.view).y + 5});
    sfSprite_setPosition(game->menu.menu_inv.item_potion_sprite, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 88,
        sfView_getCenter(game->camera.view).y + 2});
    sfSprite_setPosition(game->menu.menu_inv.item_key_sprite, (sfVector2f) {
        sfView_getCenter(game->camera.view).x - 40,
        sfView_getCenter(game->camera.view).y});
}

void menu_inventory_render(game_t *game)
{
    sfRenderWindow_drawSprite(game->window,
        game->menu.menu_inv.parallax_sprite, NULL);
    sfRenderWindow_drawSprite(game->window,
        game->menu.menu_inv.inv_sprite, NULL);
    sfRenderWindow_drawText(game->window,
        game->menu.menu_inv.title, NULL);
    if (game->player.items.map)
        sfRenderWindow_drawSprite(game->window,
            game->menu.menu_inv.item_map_sprite, NULL);
    if (game->player.items.potion)
        sfRenderWindow_drawSprite(game->window,
            game->menu.menu_inv.item_potion_sprite, NULL);
    if (game->player.items.key)
        sfRenderWindow_drawSprite(game->window,
            game->menu.menu_inv.item_key_sprite, NULL);
}

void menu_inventory_destroy(game_t *game)
{
    sfSprite_destroy(game->menu.menu_inv.parallax_sprite);
    sfTexture_destroy(game->menu.menu_inv.parallax_texture);
    sfSprite_destroy(game->menu.menu_inv.inv_sprite);
    sfTexture_destroy(game->menu.menu_inv.inv_texture);
    sfSprite_destroy(game->menu.menu_inv.item_map_sprite);
    sfSprite_destroy(game->menu.menu_inv.item_potion_sprite);
    sfSprite_destroy(game->menu.menu_inv.item_key_sprite);
    sfText_destroy(game->menu.menu_inv.title);
}