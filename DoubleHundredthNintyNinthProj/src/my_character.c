/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** character
*/

#include <stdio.h>
#include "my.h"
#include "my_defines.h"

void moving_cases(rpg_t *rpg)
{
    if (rpg->character->clock == NULL)
        rpg->character->clock = sfClock_create();
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    if (sfTime_asMilliseconds(sfClock_getElapsedTime(rpg->character->clock)) >=
        15000 / rpg->character->speed) {
        moves_dir(rpg, rect, pos, pixel_pos);
    }
}
