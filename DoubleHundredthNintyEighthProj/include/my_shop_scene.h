/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#ifndef __SHOP_SCENE__
    #define __SHOP_SCENE__
    #include <SFML_Graphics.h>
    #include <SFML_System.h>
    #include "my_list.h"

typedef struct {
    int life;
} inventory_t;

typedef struct {
    sfTexture *texture;
    int price;
    int price_multiplyer;
    char *name;
    sfVector2f scale;
} item_t;

typedef struct {
    linked_list_t *items;
} shop_data_t;

#endif
