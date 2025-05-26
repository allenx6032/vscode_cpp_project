/*
** EPITECH PROJECT, 2021
** font
** File description:
** Font Header for RPG
*/

#ifndef FONT_H_
#define FONT_H_

#include <stdbool.h>
#include <SFML_Graphics.h>
#include "engine_vector.h"

typedef enum font_type
{
    ANCIENT_MEDIUM_FONT
} font_type;

typedef struct font_s
{
    font_type type;
    sfFont *font;
} font_t;

void font_init(vector_t *fonts);
font_t *font_get(vector_t *fonts, font_type type);
void font_destroy(vector_t *fonts);

#endif /* !FONT_H_ */
