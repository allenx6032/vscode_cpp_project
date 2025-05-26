/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** text structure
*/

#ifndef TEXT_H_
    #define TEXT_H_

    #include <SFML_Graphics.h>

    #ifndef MAX_CHARACTERS
        #define MAX_CHARACTERS UINT_MAX
    #endif

typedef struct text_s {
    sfFont *font;
    sfTexture *back_ground_tex;
    sfSprite *back_ground_spr;
    sfText *text;
    sfEvent event;
    char **message;
    char *displayed_message;
    sfClock* clock;
    size_t size;
    sfVector2u text_size;
    sfRectangleShape *back_ground_rect;
    bool test;
    sfVector2u window_size;
} text_t;

unsigned int do_the_loop_in_text(text_t *text, rpg_t *rpg, unsigned int i);

#endif
