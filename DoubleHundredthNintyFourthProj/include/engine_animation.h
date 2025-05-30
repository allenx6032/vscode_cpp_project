/*
** EPITECH PROJECT, 2024
** csfml-game-engine
** File description:
** animation
*/

#ifndef INCLUDED_ANIMATION_H
    #define INCLUDED_ANIMATION_H
    #include "engine_sprite_utils.h"

struct tiles_sprite;

typedef struct animated_sprite {
    struct tiles_sprite *tiles;
    short animation_start;
    short animation_end;
    float speed;
    short current_frame;
    sfClock *clock;
} animated_sprite;

animated_sprite *create_animated_sprite(
    short start,
    short end,
    float speed,
    struct tiles_sprite *sprites
);

void set_animated_sprite_frame(
    animated_sprite *element,
    short frame
);
void update_animation(animated_sprite *element, float delta);

void set_animation(
    animated_sprite *element,
    short animation_start,
    short animation_end,
    float speed
);

#endif
