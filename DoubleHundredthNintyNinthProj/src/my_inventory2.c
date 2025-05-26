/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** inventory
*/

#include <sys/param.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_inventory.h"
#include "my_var.h"
#include "my_typedef.h"
#include "my.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))


bool use_item3(rpg_t *rpg, enum item item)
{
    switch (item) {
        case HELMET:
            add_item(rpg, HELMET, true);
            rpg->character->perso->stats->defense += 5;
            break;
        case BOOTS:
            add_item(rpg, BOOTS, true);
            rpg->character->perso->stats->defense += 2;
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool use_item2(rpg_t *rpg, enum item item)
{
    switch (item) {
        case CATNIP_ITEM:
            rpg->character->perso->stats->hp = MIN(
                10 + rpg->character->perso->stats->hp,
                rpg->character->perso->stats->hp_max);
            break;
        case MONACO_ITEM:
            rpg->character->perso->stats->strenth += 2;
            break;
        default:
            return false;
            break;
    }
    use_item3(rpg, item);
    return true;
}

bool use_item(rpg_t *rpg, enum item item)
{
    if (!item_use[item])
        return false;
    if (!remove_item(rpg, item))
        return unuse_item(rpg, item);
    switch (item) {
        case SWORD:
            add_item(rpg, SWORD, true);
            rpg->character->perso->stats->strenth += 15;
            break;
        case SHIELD:
            add_item(rpg, SHIELD, true);
            rpg->character->perso->stats->strenth -= 5;
            rpg->character->perso->stats->defense += 5;
            break;
        default:
            return false;
            break;
    }
    use_item2(rpg, item);
    return true;
}
