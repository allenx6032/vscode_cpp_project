/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** ennemy
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"
#include "my_defines.h"
#include "my_create.h"
#include "my_destroy.h"
#include "my_typedef.h"

void set_circle(rpg_t *rpg, enum cat_names name)
{
    sfFloatRect rect_black = sfSprite_getGlobalBounds(
        rpg->character->cats->sprites[name]);
    sfVector2f pos_black = {rect_black.left + rect_black.width / 2 - 200,
        rect_black.top + rect_black.height / 2 - 200};

    sfCircleShape_setPosition(rpg->character->cats->hitbox[name],
        pos_black);
    if (rpg->character->cats->is_alive[name]) {
        check_collision(rpg, name);
    }
}

void move_ennemi(rpg_t *rpg, enum cat_names name)
{
    sfFloatRect rect_white = sfSprite_getGlobalBounds(rpg->character->sprite);
    sfFloatRect rect_black = sfSprite_getGlobalBounds(
        rpg->character->cats->sprites[name]);
    sfVector2f pos_white = (sfVector2f) {
        rect_white.left + rect_white.width / 2.0f,
        rect_white.top + rect_white.height / 2.0f};
    sfVector2f pos_black = (sfVector2f) {
        rect_black.left + rect_black.width / 2.0f,
        rect_black.top + rect_black.height / 2.0f};
    sfVector2f direction = {pos_white.x - pos_black.x,
        pos_white.y - pos_black.y};
    float distance = sqrt(powf(direction.x, 2) + powf(direction.y, 2));
    sfVector2f movement = {0, 0};

    direction.x /= distance;
    direction.y /= distance;
    movement = (sfVector2f) {direction.x * 1.5, direction.y * 1.5};
    what_if_ennemy(rpg, movement, name);
}

ennemi_t setup_ennemi_t(rpg_t *rpg, enum cat_names name)
{
    ennemi_t nemi;
    nemi.pos = sfSprite_getPosition
    (rpg->character->cats->sprites[name]);
    nemi.map_pos = sfSprite_getPosition(
        rpg->menu->map->sprite[rpg->character->perso->id_zone]);
    nemi.size_char = sfSprite_getGlobalBounds
    (rpg->character->sprite);
    nemi.pixel_pos = (sfVector2f) {
        (nemi.pos.x + (nemi.size_char.width / 2.0f) -
        nemi.map_pos.x) / MAP_SCALE,
        (nemi.pos.y + (nemi.size_char.height / 2.0f) -
        nemi.map_pos.y) / MAP_SCALE};
    return nemi;
}

void check_collision(rpg_t *rpg, enum cat_names name)
{
    sfVector2f pos1_corner = sfCircleShape_getPosition(rpg->character->hitbox);
    float size1 = sfCircleShape_getRadius(rpg->character->hitbox);
    sfVector2f pos1 = (sfVector2f) {pos1_corner.x + size1,
        pos1_corner.y + size1};
    sfVector2f pos2_corner = sfCircleShape_getPosition(
        rpg->character->cats->hitbox[name]);
    float size2 = sfCircleShape_getRadius(
        rpg->character->cats->hitbox[name]);
    sfVector2f pos2 = (sfVector2f) {pos2_corner.x + size2,
        pos2_corner.y + size2};
    float dist = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
    if (dist <= MOVE_ENNEMY_DISTANCE) {
        move_ennemi(rpg, name);
    }
}
