/*
** EPITECH PROJECT, 2023
** MUL_my_hunter_
** File description:
** my_hunter.c
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
#include <SFML_Window_Window.h>
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

all_t *game_loop_bis(all_t *all, int *j, menu_t *menu)
{
    char *str = malloc(sizeof(char) * 20);

    str = "SCORE: ";
    while (sfRenderWindow_pollEvent(all->sys.wd, &all->sys.event))
        all = analyse_events(all->sys.event, all, menu);
    if (all->error == 84)
        return all;
    sfRenderWindow_clear(all->sys.wd, sfBlack);
    *all = make_time_pos(*all, *j);
    sfRenderWindow_drawSprite(all->sys.wd, all->scene.backgr.sprite, NULL);
    sfText_setString(all->scene.text.text, my_strcat(str, tostr(all->j)));
    return all;
}

void too_many_cols(all_t *all)
{
    sfRenderWindow_drawSprite(all->sys.wd, all->scene.duck.sprite, NULL);
}

void too_many_columns(all_t *all)
{
    sfSprite_setTextureRect(all->scene.pause.sprite, all->scene.pause.rect);
}

all_t *game_loop_tris(all_t *all)
{
    sfRenderWindow_drawSprite(all->sys.wd, all->scene.pause.sprite, NULL);
    sfRenderWindow_drawText(all->sys.wd, all->scene.text.text, NULL);
    all = change_mouse(all);
    sfRenderWindow_display(all->sys.wd);
    all->scene.duck.offset++;
        return all;
}

int game_loop(all_t *all, menu_t *menu)
{
    int i = 0;

    if (all->count == 1)
        sfMusic_play(all->music);
    sfRenderWindow_setMouseCursorVisible(all->sys.wd, sfFalse);
    sfRenderWindow_setFramerateLimit(all->sys.wd, 30);
    while (sfRenderWindow_isOpen(all->sys.wd)) {
        if (game_loop_bis(all, &all->j, menu)->error == 84)
            return 84;
        if (all->scene.duck.vida == 0)
            too_many_cols(all);
        sfSprite_setTextureRect(all->scene.duck.sprite, all->scene.duck.rect);
        too_many_columns(all);
        sfSprite_setTextureRect(all->scene.home.sprite, all->scene.home.rect);
        all = output_pos(all, &i);
        sfRenderWindow_drawSprite(all->sys.wd, all->scene.home.sprite, NULL);
        game_loop_tris(all);
    }
    return 0;
}
