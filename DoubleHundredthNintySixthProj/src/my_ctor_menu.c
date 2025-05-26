/*
** EPITECH PROJECT, 2023
** GOOD_VER
** File description:
** ctor_->c
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
#include <SFML_Audio.h>
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
#include <stdlib.h>
#include "my_framebuffer.h"
#include "my.h"

gameobject_t pause_menu_ctor(void)
{
    gameobject_t menu;

    menu.sprite = sfSprite_create();
    menu.texture = sfTexture_createFromFile("Sprites/menu.png", NULL);
    sfSprite_setTexture(menu.sprite, menu.texture, sfTrue);
    menu.position.x = 475;
    menu.position.y = 150;
    sfSprite_setPosition(menu.sprite, menu.position);
    return menu;
}

gameobject_t music_ctor(void)
{
    gameobject_t music;

    music.sprite = sfSprite_create();
    music.texture = sfTexture_createFromFile("Sprites/music.png", NULL);
    sfSprite_setTexture(music.sprite, music.texture, sfTrue);
    music.position.x = 950;
    music.position.y = 350;
    sfSprite_setPosition(music.sprite, music.position);
    return music;
}

gameobject_t restart_ctor(void)
{
    gameobject_t restart;

    restart.sprite = sfSprite_create();
    restart.texture = sfTexture_createFromFile("Sprites/restart.png", NULL);
    sfSprite_setTexture(restart.sprite, restart.texture, sfTrue);
    restart.position.x = 950;
    restart.position.y = 550;
    sfSprite_setPosition(restart.sprite, restart.position);
    return restart;
}

gameobject_t quit_ctor(void)
{
    gameobject_t quit;

    quit.sprite = sfSprite_create();
    quit.texture = sfTexture_createFromFile("Sprites/quit.png", NULL);
    sfSprite_setTexture(quit.sprite, quit.texture, sfTrue);
    quit.position.x = 960;
    quit.position.y = 750;
    sfSprite_setPosition(quit.sprite, quit.position);
    return quit;
}
