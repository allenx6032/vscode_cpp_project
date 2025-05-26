/*
** EPITECH PROJECT, 2021
** inventory
** File description:
** No time, no features... just bad hardcoded things...
*/

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <SFML_Graphics.h>

typedef struct inventory_items_s
{
    bool map;
    sfTexture *map_texture;
    sfIntRect map_rect;
    bool potion;
    sfTexture *potion_texture;
    sfIntRect potion_rect;
    bool key;
    sfTexture *key_texture;
    sfIntRect key_rect;
} inventory_items_t;

#endif /* !INVENTORY_H_ */