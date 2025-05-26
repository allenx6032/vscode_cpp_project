/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** text
*/

#include <stdlib.h>
#include "my.h"
#include "my_text.h"
#include "my_utils.h"
#include "my_var.h"

static void make_errored(text_t *text)
{
    sfColor color;

    sfSprite_destroy(text->back_ground_spr);
    sfTexture_destroy(text->back_ground_tex);
    text->back_ground_spr = NULL;
    text->back_ground_rect = sfRectangleShape_create();
    sfRectangleShape_setSize(text->back_ground_rect, (sfVector2f)
        {text->window_size.x, 3 * text->window_size.y / 10});
    sfRectangleShape_setPosition(text->back_ground_rect, (sfVector2f) {0,
        6.5 * text->window_size.y / 10});
    color = sfRectangleShape_getFillColor(text->back_ground_rect);
    color.a = 230;
    sfRectangleShape_setFillColor(text->back_ground_rect, color);
}

static void make_back_ground(text_t *text)
{
    sfColor color;

    if (text->back_ground_tex && text->back_ground_spr) {
        text->text_size = sfTexture_getSize(text->back_ground_tex);
        sfSprite_setTexture(text->back_ground_spr, text->back_ground_tex,
            sfTrue);
        sfSprite_setScale(text->back_ground_spr, (sfVector2f) {(float)
            text->window_size.x / (float) text->text_size.x, (float)
            text->window_size.y / (float) text->text_size.y});
        color = sfSprite_getColor(text->back_ground_spr);
        color.a = 230;
        sfSprite_setColor(text->back_ground_spr, color);
    } else {
        make_errored(text);
    }
}

static void make_text(text_t *text)
{
    text->text = sfText_create();
    sfText_setFont(text->text, text->font);
    sfText_setCharacterSize(text->text, 50);
    sfText_setString(text->text, text->displayed_message);
    sfText_setColor(text->text, sfBlack);
    sfText_setOutlineThickness(text->text, 0.2);
    sfText_setPosition(text->text, (sfVector2f){1.5 * text->window_size.x / 10,
        7 * text->window_size.y / 10});
}

static void make_struct(text_t *text, unsigned int text_num, rpg_t *rpg,
    enum speach_box who_speak)
{
    text->font = sfFont_createFromFile("assets/fonts/text_font.ttf");
    text->back_ground_tex = sfTexture_createFromFile(
        path_background_speach[who_speak], NULL);
    text->back_ground_spr = sfSprite_create();
    text->message = (char **) &(TEXT[text_num]);
    text->displayed_message = malloc_empty(sizeof(char)
        * my_strlen(TEXT[text_num]));
    text->size = my_strlen(*text->message);
    text->test = false;
    text->window_size = sfRenderWindow_getSize(rpg->window);
    make_back_ground(text);
    make_text(text);
}

void text_box(rpg_t *rpg, unsigned int text_num, enum speach_box who_speak)
{
    unsigned int i = 0;
    text_t *text = malloc_empty(sizeof(text_t));

    if (!text)
        return;
    make_struct(text, text_num, rpg, who_speak);
    text->clock = sfClock_create();
    while (sfRenderWindow_isOpen(rpg->window) && !text->test &&
        !sfKeyboard_isKeyPressed(sfKeyLControl)) {
        i = do_the_loop_in_text(text, rpg, i);
    }
    if (text->back_ground_spr) {
        sfSprite_destroy(text->back_ground_spr);
        sfTexture_destroy(text->back_ground_tex);
    } else
        sfRectangleShape_destroy(text->back_ground_rect);
    sfText_destroy(text->text);
    sfFont_destroy(text->font);
    sfClock_destroy(text->clock);
    free(text);
}
