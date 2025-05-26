/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** inventory items
*/

#ifndef INVENTORY_H_
    #define INVENTORY_H_

    #include <stdbool.h>

    enum item {
        NONE = 0,
        HELMET,
        BOOTS,
        SWORD,
        SHIELD,
        CATNIP_QUEST,
        MONACO_QUEST,
        CATNIP_ITEM,
        MONACO_ITEM
    };

typedef struct rpg rpg_t;

    typedef struct items {
        enum item name;
        bool can_be_equiped;
        bool is_equiped;
    } items_t;

bool unuse_item2(rpg_t *rpg, enum item item, int i);
bool use_item(rpg_t *rpg, enum item item);
bool use_item2(rpg_t *rpg, enum item item);
bool use_item3(rpg_t *rpg, enum item item);
bool remove_item(rpg_t *rpg, enum item item);
bool unuse_item(rpg_t *rpg, enum item item);
bool add_item(rpg_t *rpg, enum item item, bool is_equiped);

#endif
