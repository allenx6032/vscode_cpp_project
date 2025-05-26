/*
** EPITECH PROJECT, 2023
** my.h
** File description:
** struct
*/

#pragma once
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
#include <SFML_Graphics.h>
#include <SFML_Music.h>
#include <SFML_Window_Window.h>
#include <SFML_Event.h>
#include <SFML_Mouse.h>
#include <SFML_VideoMode.h>
#include <SFML_InputStream.h>
#include <SFML_Vector2.h>
#include <SFML_Config.h>
#include <SFML_Audio_Export.h>
#include <SFML_SoundStatus.h>
#include <SFML_Audio_Types.h>
#include <SFML_SoundStatus.h>
#include <SFML_InputStream.h>
#include <SFML_Time.h>
#include <SFML_Vector3.h>
#include <stddef.h>
#include <SFML_InputStream.h>
#include <SFML_Time.h>
#include <SFML_Vector3.h>
#include <stddef.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <SFML_Audio.h>

typedef struct framebuffer_s {
    unsigned int width;
    unsigned int height;
    sfUint8 *pixels;
} framebuffer_t;

typedef struct gameobject {
    int vida;
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f position;
    sfIntRect rect;
    int offset;
} gameobject_t;

typedef struct text {
    sfText *text;
    sfFont  *font;
} mytext_t;

typedef struct system {
    sfRenderWindow *wd;
    sfEvent event;
} sys_t;

typedef struct scene {
    gameobject_t duck;
    gameobject_t backgr;
    gameobject_t home;
    gameobject_t pause;
    gameobject_t menu;
    gameobject_t mouse;
    gameobject_t music;
    gameobject_t start;
    gameobject_t open;
    gameobject_t close;
    gameobject_t quit;
    mytext_t text;
} scene_t;

typedef struct all {
    scene_t scene;
    sys_t sys;
    sfClock *clock;
    int error;
    sfMusic* music;
    int j;
    int count;
} all_t;

typedef struct menuscene {
    sfSprite *sprite;
    sfTexture *texture;
} menuscene_t;

typedef struct menu {
    int isoff;
    menuscene_t scene;
    gameobject_t behind;
    gameobject_t next;
    gameobject_t button;
    int error;
    sys_t sys;
    gameobject_t music;
} menu_t;

framebuffer_t *framebuffer_create(unsigned int width, unsigned int height);
void framebuffer_destroy(framebuffer_t *framebuffer);
void close_window(sfRenderWindow *window);
int destroy_everything(all_t all, menu_t menu);

all_t make_time_pos(all_t all, int j);
void move_rect(sfIntRect *rect, int offset, int max_value);
sfIntRect assign_values(sfIntRect rect);
all_t *output_pos(all_t *all, int *i);

all_t *kill_bird(all_t *all);
all_t *analyse_events(sfEvent event, all_t *all, menu_t *menu);
all_t *manage_mouse_click(sfMouseButtonEvent event, all_t *all, menu_t *menu);

void output_help(void);

int my_put_nbr(long long int nb);
int my_put_nbr_rec(long long int nb, int a);
int my_putstr(char const *str);

all_t all_ctor(sfEvent event, menu_t menu);
mytext_t text_ctor(sfVector2f textPosition, int pos, sfColor color);

gameobject_t behind_ctor(void);
gameobject_t button_ctor(void);
menu_t menu_ctor(sfEvent event);
gameobject_t pause_ctor(void);
sys_t sys_ctor(sfEvent event, menu_t menu);
gameobject_t mouse_ctor(void);
gameobject_t pause_menu_ctor(void);
gameobject_t music_ctor(void);
gameobject_t restart_ctor(void);
gameobject_t close_ctor(void);
gameobject_t open_ctor(void);
gameobject_t quit_ctor(void);
mytext_t text_ctor(sfVector2f textPosition, int pos, sfColor color);

menu_t *analyse_menu(sfEvent event, menu_t *menu);
menu_t *is_menu(menu_t *menu);
int choose_screen(menu_t *menu, all_t *all);
all_t *part_two(sfMouseButtonEvent event, all_t *all, menu_t *menu);
all_t *part_three(sfMouseButtonEvent event, all_t *all, menu_t *menu);

void start_game(void);
int game_loop(all_t *all, menu_t *menu);
int stop_loop(all_t *all, menu_t *menu);
all_t *change_mouse(all_t *all);
void too_many_pause(all_t *all);
