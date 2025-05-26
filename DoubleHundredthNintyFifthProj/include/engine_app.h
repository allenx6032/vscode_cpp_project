/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#ifndef __APP__
    #define __APP__
    #include <SFML_Graphics.h>
    #include "engine_player.h"

typedef struct game_app {
    app_player *player;
    app_player *player2;
    sfSprite *decor;
} game_app;


game_app* init_game_app();

#endif
