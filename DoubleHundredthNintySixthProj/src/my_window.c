/*
** EPITECH PROJECT, 2023
** MUL_my_hunter_
** File description:
** my.h
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
#include <SFML_Audio.h>

framebuffer_t *framebuffer_create(unsigned int width, unsigned int height)
{
    framebuffer_t *arr = malloc(sizeof(framebuffer_t));

    arr->width = width;
    arr->height = height;
    return arr;
}

void framebuffer_destroy(framebuffer_t *arr)
{
    if (arr)
    free(arr);
}

void close_window(sfRenderWindow *window)
{
    sfRenderWindow_close(window);
}

void destroy_everything2(all_t all, menu_t menu)
{
    sfTexture_destroy(all.scene.menu.texture);
    sfTexture_destroy(all.scene.start.texture);
    sfTexture_destroy(all.scene.quit.texture);
    close_window(menu.sys.wd);
}

int destroy_everything(all_t all, menu_t menu)
{
    sfTexture_destroy(all.scene.duck.texture);
    sfTexture_destroy(all.scene.backgr.texture);
    sfTexture_destroy(all.scene.home.texture);
    sfTexture_destroy(menu.scene.texture);
    sfTexture_destroy(menu.behind.texture);
    sfTexture_destroy(menu.button.texture);
    close_window(all.sys.wd);
    destroy_everything2(all, menu);
    return 0;
}
