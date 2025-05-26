/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#ifndef __DUCK__
    #define __DUCK__
    #include <SFML_Graphics.h>
    #include <SFML_System.h>
    #include "my_entity.h"
    #include "my_animation.h"
    #include "my_default_scene.h"

typedef struct {
    sfVector2f velocity;
    animation_t *animation;
} duck_data_t;

entity_t *create_duck(
    sfVector2f pos,
    sfTexture **textures,
    default_scene_t *state
);

#endif
