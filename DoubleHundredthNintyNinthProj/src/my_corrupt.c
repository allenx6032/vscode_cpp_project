/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** test corruption in savefile
*/

#include <stdbool.h>
#include <unistd.h>
#include "my.h"
#include "my_utils.h"
#include "my_inventory.h"

static bool difference_stat(statistics_t *stat, statistics_t *stat_err)
{
    return (stat->defense != stat_err->defense
        || stat->hp != stat_err->hp
        || stat->hp_max != stat_err->hp_max
        || stat->lvl != stat_err->lvl
        || stat->strenth != stat_err->strenth
        || stat->xp != stat_err->xp
        || stat->xp_to_next_level != stat_err->xp_to_next_level);
}

static bool difference_inventory(items_t *inv, items_t *inv_err, size_t len)
{
    bool test = false;

    for (unsigned int i = 0; i < len; i++) {
        test = test || inv[i].can_be_equiped != inv_err[i].can_be_equiped
            || inv[i].is_equiped != inv_err[i].is_equiped
            || inv[i].name != inv_err[i].name;
    }
    return test;
}

bool test_corruption(char_info_t *perso, char_info_t *perso_err)
{
    bool test = false;

    if (perso->pos_x != perso_err->pos_x || perso->pos_y != perso_err->pos_y
        || perso->id_zone != perso_err->id_zone ||
        perso->old_zone != perso_err->old_zone) {
        write(2, "Load : corrupted position\n", 26);
        test = true;
    }
    if (difference_stat(perso->stats, perso_err->stats)) {
        write(2, "Load : corrupted stats\n", 23);
        test = true;
    }
    if (perso->inventory_size != perso_err->inventory_size ||
        difference_inventory(perso->inventory, perso_err->inventory,
        perso->inventory_size)) {
        write(2, "Load : corrupted inventory\n", 27);
        test = true;
    }
    return test;
}
