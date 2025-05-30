/*
** EPITECH PROJECT, 2024
** csfml-game-engine
** File description:
** sprite_utils
*/

#ifndef INCLUDED_SPRITE_UTILS_H
    #define INCLUDED_SPRITE_UTILS_H
    #include <SFML_Graphics.h>
    #include "engine_entity.h"

struct game_entity;

typedef struct tiles_sprite {
    float width;
    float height;
    sfSprite *sprite;
} tiles_sprite;


sfSprite *create_sprite_with_texture(char texture[]);

tiles_sprite *create_tiles_sprite(
    char *tiles_path,
    float sprite_height,
    float sprite_width
);
sfBool is_clicked_by_mouse(struct game_entity *entity);

void flip_sprite(sfSprite *sprite);
sfBool is_flipped(sfSprite *sprite);

#endif
