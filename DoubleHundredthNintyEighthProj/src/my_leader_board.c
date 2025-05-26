/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#include <SFML_Config.h>
#include <SFML_Graphics.h>
#include "my_scene.h"
#include "my_ui.h"
#include <SFML_System.h>

scene_t *init_leader_board_ui_scene(
    engine_t *engine
)
{
    scene_t *scene = create_scene(engine);
    entity_t *exit_button = create_exit_button(engine);

    PUSH(scene, exit_button);
    return scene;
}
