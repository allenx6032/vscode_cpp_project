/*
** EPITECH PROJECT, 2023
** my_hunter
** File description:
** ctor.c
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
#include <SFML_Audio.h>
#include <stdbool.h>
#include <SFML_Audio_Export.h>
#include <SFML_SoundStatus.h>
#include <SFML_Audio_Types.h>
#include <SFML_InputStream.h>
#include <SFML_Time.h>
#include <SFML_Vector3.h>
#include <stddef.h>
#include <SFML_Music.h>
#include "my_framebuffer.h"
#include "my.h"
#include <SFML_Audio_Export.h>
#include <SFML_SoundStatus.h>
#include <SFML_Audio_Types.h>
#include <SFML_InputStream.h>
#include <SFML_Time.h>
#include <SFML_Vector3.h>
#include <stddef.h>

gameobject_t duck_ctor(void)
{
    gameobject_t duck;
    sfIntRect rect;

    duck.sprite = sfSprite_create();
    duck.texture = sfTexture_createFromFile("Sprites/ducks.png", NULL);
    sfSprite_setTexture(duck.sprite, duck.texture, sfTrue);
    duck.rect = assign_values(rect);
    duck.offset = 0;
    duck.vida = 0;
    duck.position.x = 0;
    duck.position.y = 250;
    return duck;
}

gameobject_t background_ctor(void)
{
    gameobject_t background;

    background.sprite = sfSprite_create();
    background.texture = sfTexture_createFromFile("Sprites/FOREST.png", NULL);
    sfSprite_setTexture(background.sprite, background.texture, sfTrue);
    return background;
}

gameobject_t home_ctor(void)
{
    gameobject_t home;
    sfIntRect rect;

    home.sprite = sfSprite_create();
    home.texture = sfTexture_createFromFile("Sprites/home.png", NULL);
    sfSprite_setTexture(home.sprite, home.texture, sfTrue);
    home.rect = assign_values(rect);
    home.position.x = 1750;
    home.position.y = 40;
    sfSprite_setPosition(home.sprite, home.position);
    home.offset = 0;
    home.vida = 0;
    return home;
}

all_t all_ctor2(all_t all)
{
    all.music = sfMusic_createFromFile("Music/Knights of Favonius.ogg");
    all.count = 1;
    all.scene.close = close_ctor();
    all.scene.open = open_ctor();
    all.scene.quit = quit_ctor();
    sfMusic_play(all.music);
    return all;
}

all_t all_ctor(sfEvent event, menu_t menu)
{
    all_t all;
    sfVector2f textPosition;

    textPosition.x = 100.0f;
    textPosition.y = 50.0f;
    all.sys = sys_ctor(event, menu);
    all.scene.text = text_ctor(textPosition, 50, sfWhite);
    all.scene.duck = duck_ctor();
    all.scene.home = home_ctor();
    all.scene.pause = pause_ctor();
    all.scene.backgr = background_ctor();
    all.clock = sfClock_create();
    all.scene.mouse = mouse_ctor();
    all.scene.menu = pause_menu_ctor();
    all.scene.music = music_ctor();
    all.scene.start = restart_ctor();
    all = all_ctor2(all);
    return all;
}
