/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#ifndef __ENGINE__
    #define __ENGINE__
    #include "my_assets.h"
    #include <SFML_Config.h>
    #include <SFML_Graphics.h>
    #include <SFML_System.h>
    #include "my_list.h"


typedef struct engine {
    linked_list_t *scenes;
    int current_scene;
    sfRenderWindow *window;
    assets_server_t *assets_server;
    double delta;
    int refresh_scene;
    void *(*refresh)(struct engine *);
    sfVector2i mp;
} engine_t;

engine_t *init_game_engine(assets_server_t *assets_server);
void push_scene(engine_t *engine, void *scene);
void clear_game_engine(engine_t *engine);

#endif
