/*
** EPITECH PROJECT, 2021
** inventory_more
** File description:
** Inventory management extended
*/

#include "engine_rpg.h"
#include "engine_button.h"
#include "engine_music.h"
#include "engine_teklibc.h"

void menu_inventory_init_parallax(game_t *game)
{
    menu_inventory_t *menu_inv = &game->menu.menu_inv;

    menu_inv->parallax_sprite = sfSprite_create();
    menu_inv->parallax_texture =
        sfTexture_createFromFile("./data/loading_parallax.png", NULL);
    sfTexture_setRepeated(menu_inv->parallax_texture, sfTrue);
    sfTexture_setSmooth(menu_inv->parallax_texture, sfTrue);
    sfSprite_setTexture(menu_inv->parallax_sprite,
        menu_inv->parallax_texture, sfFalse);
    sfSprite_setScale(menu_inv->parallax_sprite, (sfVector2f){ 0.3, 0.3 });
    menu_inv->parallax_pos = (sfVector2f) {
        0 - sfView_getCenter(game->camera.view).x,
        0 - sfView_getCenter(game->camera.view).y };
    sfSprite_setPosition(menu_inv->parallax_sprite, menu_inv->parallax_pos);
    sfSprite_setTextureRect(menu_inv->parallax_sprite,
        (sfIntRect) { 0, 0, 10000, 10000});
}

void menu_inventory_update_parallax(game_t *game)
{
    sfIntRect rec = sfSprite_getTextureRect(
        game->menu.menu_inv.parallax_sprite);

    if (rec.height > 40000 || rec.width > 40000) {
        rec.height = 10000;
        rec.width = 10000;
        game->menu.menu_inv.parallax_pos = (sfVector2f) {
            0 - sfView_getSize(game->camera.view).x,
            0 - sfView_getSize(game->camera.view).y };
    }
    rec.height += 5.0;
    rec.width += 5.0;
    game->menu.menu_inv.parallax_pos.x -= 1;
    game->menu.menu_inv.parallax_pos.y -= 1;
    sfSprite_setTextureRect(game->menu.menu_inv.parallax_sprite, rec);
    sfSprite_setPosition(game->menu.menu_inv.parallax_sprite,
        game->menu.menu_inv.parallax_pos);
}

void inv_game_load(game_t *game)
{
    game->state = INVENTORY_STATE;
    sound_pause_all(&game->sounds);
    music_pause_all(&game->musics);
    music_play(&game->musics, TIME_WASTING_MUSIC, sfTrue,
        (float) my_atoi(config_get(game->config, "volume")));
}

void inv_game_unload(game_t *game)
{
    game->state = PLAYING_STATE;
    music_stop(&game->musics, TIME_WASTING_MUSIC);
    sound_resume_all(&game->sounds);
    music_resume_all(&game->musics);
}

void inv_game(game_t *game)
{
    if (game->state == PLAYING_STATE) {
        inv_game_load(game);
    } else if (game->state == INVENTORY_STATE) {
        inv_game_unload(game);
    }
}