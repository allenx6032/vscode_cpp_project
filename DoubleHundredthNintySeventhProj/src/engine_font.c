/*
** EPITECH PROJECT, 2021
** font
** File description:
** An easy way to manage font
*/

#include <stdlib.h>
#include "engine_font.h"

void font_create_intern(char *font_path, font_type type,
    vector_t *fonts)
{
    font_t *font = malloc(sizeof(font_t));

    if (font == NULL)
        return;
    font->font = sfFont_createFromFile(font_path);
    font->type = type;
    my_vector_add(fonts, font);
}

void font_init(vector_t *fonts)
{
    my_vector_init(fonts);
    font_create_intern("./data/font/ancient_medium.ttf",
        ANCIENT_MEDIUM_FONT, fonts);
}

font_t *font_get(vector_t *fonts, font_type type)
{
    font_t *font;

    for (int i = 0; i != fonts->size; i++) {
        font = my_vector_get(fonts, i);
        if (font->type == type)
            return font;
    }
    return NULL;
}

void font_destroy(vector_t *fonts)
{
    font_t *font;

    for (int i = 0; i != fonts->size; i++) {
        font = my_vector_get(fonts, i);
        sfFont_destroy(font->font);
        free(font);
    }
    my_vector_free(fonts);
}