/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** collisions
*/

#include "my.h"
#include "my_utils.h"
#include "my_defines.h"

void red_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_red(rpg, pixel_pos, rect);
}

void green_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_green(rpg, pixel_pos, rect);
}

void yellow_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_yellow(rpg, pixel_pos, rect);
}

void pink_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_pink(rpg, pixel_pos, rect);
}

void blue_collision(rpg_t *rpg)
{
    sfVector2f pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    sfFloatRect size_char = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite);
    sfVector2f pixel_pos = (sfVector2f) {
        (pos.x + (size_char.width / 2.0f) - map_pos.x) / MAP_SCALE,
        (pos.y + (size_char.height / 2.0f) - map_pos.y) / MAP_SCALE};

    case_blue(rpg, pixel_pos, rect);
}
