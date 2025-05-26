/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** character
*/

#include <stdio.h>
#include "my.h"
#include "my_defines.h"

static void move_down(rpg_t *rpg, sfIntRect rect, sfVector2f pos,
    sfVector2f pixel_pos[2])
{
    if ((sfKeyboard_isKeyPressed(sfKeyDown) || sfKeyboard_isKeyPressed(sfKeyS))
        && sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
        [rpg->character->perso->id_zone + 1],
            pixel_pos[0].x, pixel_pos[0].y)) != sfColor_toInteger(sfBlack)
            && sfColor_toInteger(sfImage_getPixel(
            rpg->menu->map->image[rpg->character->perso->id_zone + 1],
            pixel_pos[1].x, pixel_pos[1].y)) != sfColor_toInteger(sfBlack)) {
        sfClock_restart(rpg->character->clock);
        rect.top = rect.height * 0;
        pos.y += 10;
        rect.left += rect.width;
        if (rect.left >= (rect.width * 3))
            rect.left = 0;
        sfSprite_setPosition(rpg->character->sprite, pos);
        sfSprite_setTextureRect(rpg->character->sprite, rect);
        sfCircleShape_setPosition(rpg->character->hitbox, pos);
    }
}

static void move_up(rpg_t *rpg, sfIntRect rect, sfVector2f pos,
    sfVector2f pixel_pos[2])
{
    if ((sfKeyboard_isKeyPressed(sfKeyUp) || sfKeyboard_isKeyPressed(sfKeyZ))
        && sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
        [rpg->character->perso->id_zone + 1],
            pixel_pos[0].x, pixel_pos[0].y)) != sfColor_toInteger(sfBlack)
            && sfColor_toInteger(sfImage_getPixel(
            rpg->menu->map->image[rpg->character->perso->id_zone + 1],
            pixel_pos[1].x, pixel_pos[1].y)) != sfColor_toInteger(sfBlack)) {
        sfClock_restart(rpg->character->clock);
        rect.top = rect.height * 3;
        pos.y -= 10;
        rect.left += rect.width;
        if (rect.left >= (rect.width * 3))
            rect.left = 0;
        sfSprite_setPosition(rpg->character->sprite, pos);
        sfSprite_setTextureRect(rpg->character->sprite, rect);
        sfCircleShape_setPosition(rpg->character->hitbox, pos);
    }
}

static void move_left(rpg_t *rpg, sfIntRect rect, sfVector2f pos,
    sfVector2f pixel_pos[2])
{
    if ((sfKeyboard_isKeyPressed(sfKeyLeft) || sfKeyboard_isKeyPressed(sfKeyQ))
        && sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
        [rpg->character->perso->id_zone + 1],
            pixel_pos[0].x, pixel_pos[0].y)) != sfColor_toInteger(sfBlack)
            && sfColor_toInteger(sfImage_getPixel(
            rpg->menu->map->image[rpg->character->perso->id_zone + 1],
            pixel_pos[1].x, pixel_pos[1].y)) != sfColor_toInteger(sfBlack)) {
        sfClock_restart(rpg->character->clock);
        rect.top = rect.height * 1;
        pos.x -= 10;
        rect.left += rect.width;
        if (rect.left >= (rect.width * 3))
            rect.left = 0;
        sfSprite_setPosition(rpg->character->sprite, pos);
        sfSprite_setTextureRect(rpg->character->sprite, rect);
        sfCircleShape_setPosition(rpg->character->hitbox, pos);
    }
}

static void move_right(rpg_t *rpg, sfIntRect rect, sfVector2f pos,
    sfVector2f pixel_pos[2])
{
    if ((sfKeyboard_isKeyPressed(sfKeyRight) || sfKeyboard_isKeyPressed(sfKeyD))
        && sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
        [rpg->character->perso->id_zone + 1],
            pixel_pos[0].x, pixel_pos[0].y)) != sfColor_toInteger(sfBlack)
            && sfColor_toInteger(sfImage_getPixel(
            rpg->menu->map->image[rpg->character->perso->id_zone + 1],
            pixel_pos[1].x, pixel_pos[1].y)) != sfColor_toInteger(sfBlack)) {
        sfClock_restart(rpg->character->clock);
        rect.top = rect.height * 2;
        pos.x += 10;
        rect.left += rect.width;
        if (rect.left >= (rect.width * 3))
            rect.left = 0;
        sfSprite_setPosition(rpg->character->sprite, pos);
        sfSprite_setTextureRect(rpg->character->sprite, rect);
        sfCircleShape_setPosition(rpg->character->hitbox, pos);
    }
}

void moves_dir(rpg_t *rpg, sfIntRect rect, sfVector2f pos, sfVector2f pixel_pos)
{
    move_up(rpg, rect, pos, (sfVector2f [2]) {
        (sfVector2f) {pixel_pos.x - (rect.width / 2 / MAP_SCALE),
        pixel_pos.y - ((rect.height + 20) / 2 / MAP_SCALE)},
        (sfVector2f) {pixel_pos.x + (rect.width / 2 / MAP_SCALE),
        pixel_pos.y - ((rect.height + 20) / 2 / MAP_SCALE)}});
    move_right(rpg, rect, pos, (sfVector2f [2]) {
        (sfVector2f) {pixel_pos.x + ((rect.width + 40) / 2 / MAP_SCALE),
        pixel_pos.y - (rect.height / 2 / MAP_SCALE)},
        (sfVector2f) {pixel_pos.x + ((rect.width + 40) / 2 / MAP_SCALE),
        pixel_pos.y + (rect.height / 2 / MAP_SCALE)}});
    move_down(rpg, rect, pos, (sfVector2f [2]) {
        (sfVector2f) {pixel_pos.x + (rect.width / 2 / MAP_SCALE),
        pixel_pos.y + ((rect.height + 80) / 2 / MAP_SCALE)},
        (sfVector2f) {pixel_pos.x - (rect.width / 2 / MAP_SCALE),
        pixel_pos.y + ((rect.height + 80) / 2 / MAP_SCALE)}});
    move_left(rpg, rect, pos, (sfVector2f [2]) {
        (sfVector2f) {pixel_pos.x - ((rect.width + 40) / 2 / MAP_SCALE),
        pixel_pos.y - (rect.height / 2 / MAP_SCALE)},
        (sfVector2f) {pixel_pos.x - ((rect.width + 40) / 2 / MAP_SCALE),
        pixel_pos.y + (rect.height / 2 / MAP_SCALE)}});
}
