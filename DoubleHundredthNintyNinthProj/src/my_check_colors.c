/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** check_colors
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"

bool check_red(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(237, 28, 36));
}

bool check_green(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(34, 177, 76));
}

bool check_yellow(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(255, 242, 0));
}

bool check_pink(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(255, 174, 201));
}

bool check_blue(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(0, 162, 232));
}
