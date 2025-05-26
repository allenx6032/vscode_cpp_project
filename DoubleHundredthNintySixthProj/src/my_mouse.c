/*
** EPITECH PROJECT, 2023
** MUL_my_hunter_
** File description:
** mouse.c
*/

#include <SFML_RenderWindow.h>
#include <SFML_BlendMode.h>
#include <SFML_Transform.h>
#include <SFML_Graphics_Export.h>
#include <SFML_Color.h>
#include <SFML_Graphics_Types.h>
#include <SFML_Rect.h>
#include <SFML_Event.h>
#include <SFML_Mouse.h>
#include <SFML_VideoMode.h>
#include <SFML_InputStream.h>
#include <SFML_Vector2.h>
#include <SFML_Config.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_framebuffer.h"
#include "my.h"

all_t *kill_bird(all_t *all)
{
    all->scene.duck.vida = 1;
    all->j++;
    return all;
}

all_t *manage_mouse_click(sfMouseButtonEvent event, all_t *all, menu_t *menu)
{
    sfFloatRect Bound = sfSprite_getGlobalBounds(all->scene.duck.sprite);
    sfFloatRect Boundbis = sfSprite_getGlobalBounds(all->scene.home.sprite);

    if (menu->isoff != 5 && sfFloatRect_contains(&Bound, event.x, event.y))
        all = kill_bird(all);
    if (sfFloatRect_contains(&Boundbis, event.x, event.y)) {
        menu->isoff = 3;
        sfMusic_destroy(all->music);
        all->music = sfMusic_createFromFile("Music/Knights of Favonius.ogg");
        sfMusic_play(all->music);
        all->scene.duck.position.x = 0;
        all->j = 0;
        if (choose_screen(menu, all) == 84) {
            all->error = 84;
            return all;
        }
    }
    all = part_two(event, all, menu);
    return all;
}

all_t *analyse_events(sfEvent event, all_t *all, menu_t *menu)
{
    sfMouseButtonEvent eventito = event.mouseButton;

    if (event.type == sfEvtClosed) {
        destroy_everything(*all, *menu);
        all->error = 84;
        return all;
    }
    all->error = 0;
    if (event.type == sfEvtMouseButtonPressed)
        all = manage_mouse_click(eventito, all, menu);
    return all;
}

menu_t *see_mouse_menu(sfMouseButtonEvent event, menu_t *menu)
{
    sfFloatRect spriteBound = sfSprite_getGlobalBounds(menu->button.sprite);

    if (sfFloatRect_contains(&spriteBound, event.x, event.y))
        menu->isoff = 1;
    return menu;
}

menu_t *analyse_menu(sfEvent event, menu_t *menu)
{
    sfMouseButtonEvent eventito = event.mouseButton;

    if (event.type == sfEvtClosed) {
        menu->isoff = 2;
        close_window(menu->sys.wd);
        menu->error = 84;
        return menu;
    }
    if (event.type == sfEvtMouseButtonPressed)
        menu = see_mouse_menu(eventito, menu);
    return menu;
}
