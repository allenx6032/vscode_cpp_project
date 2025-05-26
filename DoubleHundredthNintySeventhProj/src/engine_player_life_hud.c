/*
** EPITECH PROJECT, 2021
** player_life_hud
** File description:
** Player Life HUD basic game calls
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_player.h"

void player_life_hud_init_more(player_life_hud_t *life_hud)
{
    sfSprite_setTexture(life_hud->icon_sprite,
        life_hud->texture, sfFalse);
    sfSprite_setTexture(life_hud->life_hud_sprite,
        life_hud->texture, sfFalse);
    sfSprite_setTexture(life_hud->life_box_sprite,
        life_hud->texture, sfFalse);
    sfSprite_setTextureRect(life_hud->icon_sprite,
        (sfIntRect){ 0, 0, 31, 31});
    sfSprite_setTextureRect(life_hud->life_hud_sprite,
        (sfIntRect){ 32, 7, 43, 13});
    sfSprite_setTextureRect(life_hud->life_box_sprite,
        (sfIntRect){ 32, 20, 43, 12});
}

void player_life_hud_init(player_life_hud_t *life_hud)
{
    life_hud->icon_sprite = sfSprite_create();
    life_hud->life_hud_sprite = sfSprite_create();
    life_hud->life_box_sprite = sfSprite_create();
    life_hud->texture = sfTexture_createFromFile(
        "./data/player_life_hud.png", NULL);
    player_life_hud_init_more(life_hud);
    sfSprite_setScale(life_hud->icon_sprite, (sfVector2f) {1.5, 1.5});
    sfSprite_setScale(life_hud->life_hud_sprite,
        (sfVector2f) {1.5, 1.5});
    sfSprite_setScale(life_hud->life_box_sprite,
        (sfVector2f) {1.5, 1.5});
}

void player_life_hud_update(player_t *player, sfView *camera)
{
    const float top_x = sfView_getCenter(camera).x
        - (sfView_getSize(camera).x / 2);
    const float top_y = sfView_getCenter(camera).y
        - (sfView_getSize(camera).y / 2);
    sfIntRect life_box_rec = sfSprite_getTextureRect(
        player->life_hud.life_box_sprite);

    sfSprite_setPosition(player->life_hud.icon_sprite,
        (sfVector2f) {top_x + 20, top_y + 20});
    sfSprite_setPosition(player->life_hud.life_hud_sprite,
        (sfVector2f) {top_x + 20 + (31 * 1.5), top_y + 20 + 15});
    life_box_rec.width = (int) 43 * (player->life * 0.01);
    if (life_box_rec.width < 2 && player->life > 0)
        life_box_rec.width = 2;
    sfSprite_setTextureRect(player->life_hud.life_box_sprite,
        life_box_rec);
    sfSprite_setPosition(player->life_hud.life_box_sprite,
        (sfVector2f) {top_x + 20 + (31 * 1.5), top_y + 20 + 15});
}

void player_life_hud_render(sfRenderWindow *window, player_life_hud_t life_hud)
{
    sfRenderWindow_drawSprite(window, life_hud.icon_sprite, NULL);
    sfRenderWindow_drawSprite(window, life_hud.life_hud_sprite, NULL);
    sfRenderWindow_drawSprite(window, life_hud.life_box_sprite, NULL);
}

void player_life_hud_destroy(player_life_hud_t *life_hud)
{
    sfSprite_destroy(life_hud->icon_sprite);
    sfSprite_destroy(life_hud->life_hud_sprite);
    sfSprite_destroy(life_hud->life_box_sprite);
    sfTexture_destroy(life_hud->texture);
}