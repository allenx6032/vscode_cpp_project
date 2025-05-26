/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** goto
*/

#include "my.h"
#include "my_defines.h"
#include "my_positions.h"
#include <stdbool.h>
#include "my_var.h"

void goto_house1(rpg_t *rpg)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = HOUSE1;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[HOUSE1], HOUSE1_POS_MAP);
    sfSprite_setPosition(rpg->menu->map->sprite[HOUSE1_PATH], HOUSE1_POS_MAP);
    sfSprite_setPosition(rpg->character->sprite, HOUSE1_POS_CHAR);
    sfCircleShape_setPosition(rpg->character->hitbox, HOUSE1_POS_CHAR);
    sfSprite_setScale(rpg->character->monaco_spr, (sfVector2f){0, 0});
}

void goto_map1(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = MAP_1;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
        sfSprite_setPosition(rpg->character->cats->sprites[i],
        (sfVector2f){500, 500});
    }
    sfSprite_setPosition(rpg->menu->map->sprite[MAP_1], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[MAP_1_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
    sfSprite_setScale(rpg->character->monaco_spr, (sfVector2f){0, 0});
}

void goto_inn(rpg_t *rpg)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = INN;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[INN], INN_POS_MAP);
    sfSprite_setPosition(rpg->menu->map->sprite[INN_PATH], INN_POS_MAP);
    sfSprite_setPosition(rpg->character->sprite, INN_POS_CHAR);
    sfCircleShape_setPosition(rpg->character->hitbox, INN_POS_CHAR);
    sfSprite_setScale(rpg->character->monaco_spr, (sfVector2f){0, 0});
}

void goto_bar(rpg_t *rpg)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = BAR;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[BAR], BAR_POS_MAP);
    sfSprite_setPosition(rpg->menu->map->sprite[BAR_PATH], BAR_POS_MAP);
    sfSprite_setPosition(rpg->character->sprite, BAR_POS_CHAR);
    sfCircleShape_setPosition(rpg->character->hitbox, BAR_POS_CHAR);
    sfSprite_setPosition(rpg->character->monaco_spr, (sfVector2f){750, -150});
}

void goto_map2(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = MAP_2;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
        sfSprite_setPosition(rpg->character->cats->sprites[i],
        (sfVector2f){500, 500});
    }
    sfSprite_setPosition(rpg->menu->map->sprite[MAP_2], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[MAP_2_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}
