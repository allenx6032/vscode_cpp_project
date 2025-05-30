/*
** EPITECH PROJECT, 2024
** csfml-game-engine
** File description:
** fps_display
*/

#ifndef INCLUDED_FPS_DISPLAY_H
    #define INCLUDED_FPS_DISPLAY_H
    #include <SFML_Graphics.h>
    #include "engine_entity.h"

game_entity *create_fps_displayer(sfFont *font, struct game_engine *engine);

#endif
