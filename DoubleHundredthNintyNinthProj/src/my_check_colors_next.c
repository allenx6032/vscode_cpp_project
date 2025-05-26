/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** check_colors_next
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"

bool check_purple(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(128, 0, 255));
}

bool check_grey(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(127, 127, 127));
}

bool check_orange(rpg_t *rpg, sfVector2f pixel_pos)
{
    return sfColor_toInteger(sfImage_getPixel(
    rpg->menu->map->image[rpg->character->perso->id_zone + 1],
    pixel_pos.x, pixel_pos.y)) ==
    sfColor_toInteger(sfColor_fromRGB(255, 127, 39));
}
