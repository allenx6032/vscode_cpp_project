/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** goto_final
*/

#include "my.h"
#include "my_positions.h"
#include "my_defines.h"
#include "my_var.h"

void goto_armor(rpg_t *rpg)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = ARMOR;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[ARMOR], ARMOR_POS_MAP);
    sfSprite_setPosition(rpg->menu->map->sprite[ARMOR_PATH], ARMOR_POS_MAP);
    sfSprite_setPosition(rpg->character->sprite, ARMOR_POS_CHAR);
    sfCircleShape_setPosition(rpg->character->hitbox, ARMOR_POS_CHAR);
}

void goto_weapon(rpg_t *rpg)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = WEAPON;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[WEAPON], WEAPON_POS_MAP);
    sfSprite_setPosition(rpg->menu->map->sprite[WEAPON_PATH], WEAPON_POS_MAP);
    sfSprite_setPosition(rpg->character->sprite, WEAPON_POS_CHAR);
    sfCircleShape_setPosition(rpg->character->hitbox, WEAPON_POS_CHAR);
}
