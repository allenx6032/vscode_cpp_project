/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#include "engine_event_handling.h"
#include <stdio.h>
#include <SFML_Graphics.h>

void check_event(sfRenderWindow *window, sfEvent *event)
{
    if (event->type == sfEvtClosed) {
        sfRenderWindow_close(window);
    }
}
