/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** inventory
*/

#include <sys/param.h>
#include "my_inventory.h"
#include "my_var.h"
#include "my_typedef.h"
#include "my.h"

bool add_item(rpg_t *rpg, enum item item, bool is_equiped)
{
    unsigned int i = 0;

    while (i < rpg->character->perso->inventory_size
        && rpg->character->perso->inventory[i].name != NONE)
        i++;
    if (i == rpg->character->perso->inventory_size)
        return false;
    rpg->character->perso->inventory[i].name = item;
    rpg->character->perso->inventory[i].can_be_equiped = item_use[item];
    rpg->character->perso->inventory[i].is_equiped = is_equiped;
    return true;
}

bool remove_item(rpg_t *rpg, enum item item)
{
    unsigned int i = 0;

    while (i < rpg->character->perso->inventory_size
        && (rpg->character->perso->inventory[i].name != item
        || rpg->character->perso->inventory[i].is_equiped))
        i++;
    if (i == rpg->character->perso->inventory_size)
        return false;
    rpg->character->perso->inventory[i].name = NONE;
    rpg->character->perso->inventory[i].can_be_equiped = false;
    return true;
}

bool unuse_item2(rpg_t *rpg, enum item item, int i)
{
    switch (item) {
        case HELMET:
            rpg->character->perso->inventory[i].is_equiped = false;
            rpg->character->perso->stats->defense -= 10;
            break;
        case BOOTS:
            rpg->character->perso->inventory[i].is_equiped = false;
            rpg->character->perso->stats->defense -= 5;
            break;
        case SWORD:
            rpg->character->perso->inventory[i].is_equiped = false;
            rpg->character->perso->stats->strenth -= 15;
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool unuse_item(rpg_t *rpg, enum item item)
{
    unsigned int i = 0;

    while (i < rpg->character->perso->inventory_size
        && (rpg->character->perso->inventory[i].name != item
        || !rpg->character->perso->inventory[i].is_equiped))
        i++;
    if (i == rpg->character->perso->inventory_size)
        return false;
    switch (item) {
        case SHIELD:
            rpg->character->perso->inventory[i].is_equiped = false;
            rpg->character->perso->stats->defense -= 10;
            rpg->character->perso->stats->strenth += 5;
            break;
        default:
            return false;
            break;
    }
    unuse_item2(rpg, item, i);
    return true;
}
