/*
** EPITECH PROJECT, 2023
** GOOD_VER
** File description:
** main.c
*/
#include <SFML_PrimitiveType.h>
#include <SFML_RenderWindow.h>
#include <SFML_RenderStates.h>
#include <SFML_WindowHandle.h>
#include <SFML_BlendMode.h>
#include <SFML_Transform.h>
#include <SFML_Vertex.h>
#include <SFML_Graphics_Export.h>
#include <SFML_Sprite.h>
#include <SFML_Color.h>
#include <SFML_Graphics_Types.h>
#include <SFML_Color.h>
#include <SFML_Rect.h>
#include <SFML_Window.h>
#include <SFML_Event.h>
#include <SFML_Mouse.h>
#include <SFML_VideoMode.h>
#include <SFML_InputStream.h>
#include <SFML_Vector2.h>
#include <SFML_Config.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_framebuffer.h"
#include "my.h"
#include <SFML_Audio.h>

int choose_screen(menu_t *menu, all_t *all)
{
    if (menu->error == 84) {
        return 84;
    }
    if (menu->isoff == 3) {
        menu = is_menu(menu);
    }
    if (menu->isoff == 2) {
        game_loop(all, menu);
    }
    if (menu->isoff == 5) {
        if (stop_loop(all, menu) == 84)
            return 84;
    }
    return 0;
}

void start_game(void)
{
    sfEvent event;
    all_t all;
    menu_t *menu = malloc(sizeof(menu_t));

    *menu = menu_ctor(event);
    menu = is_menu(menu);
    if (menu->error == 84)
        return;
    if (menu->isoff == 2)
        return;
    all = all_ctor(event, *menu);
    all.j = 0;
    if (game_loop(&all, menu) == 84)
        return;
    return;
}

int main(int ac, char **av)
{
    if (ac == 0)
        return 84;
    if (ac == 2) {
        if (my_strcmp(av[1], "-h") == true)
            output_help();
        return 84;
    }
    if (ac > 2)
        return 84;
    start_game();
    return 0;
}
