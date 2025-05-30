/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#ifndef __UI__
    #define __UI__
    #include "my_entity.h"
    #include <SFML_Graphics.h>
    #include "my_shop_scene.h"

entity_t *create_exit_button(engine_t *engine);
void item_card(
    scene_t *scene,
    sfVector2f pos,
    item_t *item
);
#endif
