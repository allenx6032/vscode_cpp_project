/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** map_update
*/

#include "my_positions.h"
#include "my.h"
#include "my_defines.h"

static void make_floats(float floats[5], sfVector2u window_size)
{
    floats[0] = MARGIN;
    floats[1] = window_size.x * floats[0];
    floats[2] = window_size.x - floats[1];
    floats[3] = window_size.y * floats[0];
    floats[4] = window_size.y - floats[3];
}

static void horizontal_direction(float floats[5], sfVector2f pos[2],
    sfVector2u window_size, sfFloatRect map_size)
{
    if (pos[1].x < floats[1] && pos[0].x < 0) {
        pos[0].x += 10.0f;
        pos[1].x += 10.0f;
    } else if (pos[1].x > floats[2] &&
        pos[0].x + map_size.width > window_size.x) {
        pos[0].x -= 10.0f;
        pos[1].x -= 10.0f;
    }
}

static void vertical_direction(float floats[5], sfVector2f pos[2],
    sfVector2u window_size, sfFloatRect map_size)
{
    if (pos[1].y < floats[3] && pos[0].y < 0) {
        pos[0].y += 10.0f;
        pos[1].y += 10.0f;
    } else if (pos[1].y > floats[4] &&
        pos[0].y + map_size.height > window_size.y) {
        pos[0].y -= 10.0f;
        pos[1].y -= 10.0f;
    }
}

static void map_move_enemies(sfVector2f enemy_move, rpg_t *rpg)
{
    sfVector2f pos = (sfVector2f) {0, 0};
    sfFloatRect size = (sfFloatRect) {0, 0, 0, 0};
    float radius = 0;

    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        pos = sfSprite_getPosition(rpg->character->cats->sprites[i]);
        radius = sfCircleShape_getRadius(rpg->character->cats->hitbox[i]);
        size = sfSprite_getGlobalBounds(rpg->character->cats->sprites[i]);
        pos.x += enemy_move.x;
        pos.y += enemy_move.y;
        sfSprite_setPosition(rpg->character->cats->sprites[i], pos);
        pos.x -= radius;
        pos.y -= radius;
        pos.x += size.width / 2;
        pos.y += size.height / 2;
        sfCircleShape_setPosition(rpg->character->cats->hitbox[i], pos);
    }
}

void update_map_position(rpg_t *rpg, enum map_names map)
{
    sfVector2f character_pos = sfSprite_getPosition(rpg->character->sprite);
    sfVector2f map_pos = sfSprite_getPosition(rpg->menu->map->sprite[map]);
    sfVector2u window_size = sfRenderWindow_getSize(rpg->window);
    sfVector2f enemy_move = (sfVector2f) {0, 0};
    sfFloatRect map_size = sfSprite_getGlobalBounds(
        rpg->menu->map->sprite[map]);
    float floats[5] = {0, 0, 0, 0, 0};
    sfVector2f pos[2] = {map_pos, character_pos};

    make_floats(floats, window_size);
    horizontal_direction(floats, pos, window_size, map_size);
    vertical_direction(floats, pos, window_size, map_size);
    sfSprite_setPosition(rpg->menu->map->sprite[map], pos[0]);
    sfSprite_setPosition(rpg->menu->map->sprite[map + 1], pos[0]);
    sfSprite_setPosition(rpg->character->sprite, pos[1]);
    enemy_move.x = pos[0].x - map_pos.x;
    enemy_move.y = pos[0].y - map_pos.y;
    map_move_enemies(enemy_move, rpg);
    map_move_monaco(enemy_move, rpg);
}
