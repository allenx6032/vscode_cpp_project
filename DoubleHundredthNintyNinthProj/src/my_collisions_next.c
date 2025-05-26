/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** collisions_next
*/

#include "my.h"
#include "my_utils.h"
#include "my_defines.h"

void purple_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_purple(rpg, pixel_pos, rect);
}

void grey_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_grey(rpg, pixel_pos, rect);
}

void orange_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_orange(rpg, pixel_pos, rect);
}
