/*
** EPITECH PROJECT, 2023
** is escaped
** File description:
** is escaped
*/

#include <SFML_Graphics.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "my_menu.h"
#include "my_typedef.h"
#include "my_utils.h"
#include "my_create.h"

void is_escaped(sfEvent *event, game_menu_t *game)
{
    if (event->type == sfEvtKeyPressed) {
        if (event->key.code == sfKeyEscape) {
            game->start_button_pressed = false;
        }
    }
}
