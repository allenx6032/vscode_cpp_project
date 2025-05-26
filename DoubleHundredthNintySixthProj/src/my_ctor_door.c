/*
** EPITECH PROJECT, 2023
** GOOD_VER
** File description:
** ctor_door.c
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

gameobject_t open_ctor(void)
{
    gameobject_t open;

    open.sprite = sfSprite_create();
    open.texture = sfTexture_createFromFile("Sprites/open.png", NULL);
    sfSprite_setTexture(open.sprite, open.texture, sfTrue);
    open.position.x = 1650;
    open.position.y = 900;
    sfSprite_setPosition(open.sprite, open.position);
    return open;
}

gameobject_t close_ctor(void)
{
    gameobject_t close;

    close.sprite = sfSprite_create();
    close.texture = sfTexture_createFromFile("Sprites/close.png", NULL);
    sfSprite_setTexture(close.sprite, close.texture, sfTrue);
    close.position.x = 1650;
    close.position.y = 900;
    sfSprite_setPosition(close.sprite, close.position);
    return close;
}
