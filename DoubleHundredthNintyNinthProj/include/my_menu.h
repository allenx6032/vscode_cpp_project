/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** menu
*/

#ifndef MENU_H_
    #define MENU_H_

    #include <SFML_Audio.h>
    #include <SFML_Graphics.h>
    #include <stdbool.h>
    #include "my_typedef.h"

    typedef struct menu_video_s {
        sfRenderWindow *window;
        sfFloatRect yes_bounds;
        sfFloatRect no_bounds;
        sfFloatRect start_bounds;
        sfFloatRect settings_bounds;
        sfFloatRect exit_bounds;
    } menu_video_t;

    typedef struct menu_audio_s {
        sfMusic *menu_song;
        sfSoundBuffer *meow_buffer;
        sfSoundBuffer *click_buffer;
        sfSound *meow_click;
        sfSound *menu_click;
    } menu_audio_t;

    typedef struct menu_sprites_s {
        sfTexture *backgroundTexture;
        sfSprite *backgroundSprite;
        sfTexture *start_texture;
        sfTexture *settings_texture;
        sfTexture *exit_texture;
        sfSprite *start_button;
        sfSprite *settings_button;
        sfSprite *exit_button;
        sfRectangleShape *yes_button;
        sfRectangleShape *no_button;
        sfText *title_text;
        sfFont *font;
        sfText *resume_text;
        sfText *no_text;
        sfText *yes_text;
        sfSprite *background;
    } menu_sprites_t;

    typedef struct game_menu_s {
        menu_video_t *video;
        menu_audio_t *audio;
        menu_sprites_t *sprites;
        sfBool start_button_pressed;
    } game_menu_t;

bool menu(rpg_t *rpg);
void resize_menu(game_menu_t *game, sfEvent *event);
void create_menu_part_1(game_menu_t *game);
void create_menu_part_4_point_5(game_menu_t *game);
void create_menu_part_3_point_5(game_menu_t *game);
void create_menu_part_1_point_5(game_menu_t *game);
bool main_loop(game_menu_t *game, sfEvent *event, rpg_t *rpg);
void is_escaped(sfEvent *event, game_menu_t *game);
void exit_bounds_cond(game_menu_t *game, sfEvent *event);
void settings_bounds_cond(game_menu_t *game, sfEvent *event);
void start_bounds_cond(game_menu_t *game, sfEvent *event);
bool event_loop(game_menu_t *game, sfEvent *event, rpg_t *rpg);

#endif
