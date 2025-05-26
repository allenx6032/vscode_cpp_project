/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** loads
*/

#include <stdlib.h>
#include "my_utils.h"
#include "my.h"

static void print_screen(sfRenderWindow *window, sfText *text,
    sfRectangleShape *bar[2])
{
    sfVector2u screen_size = sfRenderWindow_getSize(window);
    sfVector2f middle = (sfVector2f) {screen_size.x / 2.0, screen_size.y / 2.0};
    sfFloatRect text_rect = sfText_getGlobalBounds(text);
    sfFloatRect bar_rect = sfRectangleShape_getGlobalBounds(bar[0]);

    sfRectangleShape_setFillColor(bar[0], sfTransparent);
    sfRectangleShape_setFillColor(bar[1], sfWhite);
    sfRectangleShape_setOutlineColor(bar[0], sfWhite);
    sfText_setPosition(text, (sfVector2f) {middle.x - (text_rect.width / 2.0f)
        - 20, middle.y - bar_rect.height - (text_rect.height / 2.0f)});
    sfRenderWindow_clear(window, sfBlack);
    sfRenderWindow_drawText(window, text, NULL);
    sfRenderWindow_drawRectangleShape(window, bar[0], NULL);
    sfRenderWindow_drawRectangleShape(window, bar[1], NULL);
    sfRenderWindow_display(window);
}

static size_t get_percent_size(int percent)
{
    size_t size = 0;

    size += my_strlen("Loading: ?%...");
    if (percent < 0) {
        size++;
        percent = -percent;
    }
    while (percent > 10) {
        percent = percent / 10;
        size++;
    }
    size++;
    return size;
}

static char *make_percent_string(int percent)
{
    size_t percent_size = get_percent_size(percent);
    char *str = malloc_empty(sizeof(char) * (percent_size + 2));
    char *str_nbr = my_strnbr_base(percent, "0123456789");

    if (str == NULL || str_nbr == NULL) {
        free(str);
        free(str_nbr);
        return NULL;
    }
    my_strcpy(str, "Loading: ");
    my_strcat(str, str_nbr);
    free(str_nbr);
    my_strcat(str, "%...");
    return str;
}

static void print_string_progression(sfRenderWindow *window,
    int percent, sfRectangleShape *bar[2])
{
    sfFont *font = sfFont_createFromFile("assets/fonts/arial.ttf");
    sfText *percent_text = sfText_create();
    char *str = make_percent_string(percent);

    sfText_setFont(percent_text, font);
    sfText_setCharacterSize(percent_text, 50);
    sfText_setString(percent_text, str);
    sfText_setColor(percent_text, sfWhite);
    sfText_setOutlineThickness(percent_text, -50);
    print_screen(window, percent_text, bar);
    sfText_destroy(percent_text);
    sfFont_destroy(font);
    free(str);
    sfRectangleShape_destroy(bar[0]);
    sfRectangleShape_destroy(bar[1]);
}

void print_progression(sfRenderWindow *window, int percent)
{
    sfRectangleShape *bars[2] = {NULL, NULL};
    sfVector2u size = sfRenderWindow_getSize(window);
    sfVector2f pos = {size.x / 2 - 260, size.y / 2 + 10};

    bars[0] = sfRectangleShape_create();
    bars[1] = sfRectangleShape_create();
    sfRectangleShape_setOutlineThickness(bars[0], 5);
    sfRectangleShape_setSize(bars[0], (sfVector2f) {510, 60});
    sfRectangleShape_setPosition(bars[0], (sfVector2f) {pos.x, pos.y});
    if (percent >= 0) {
        sfRectangleShape_setSize(bars[1], (sfVector2f) {5 * percent, 50});
        sfRectangleShape_setPosition(bars[1],
            (sfVector2f) {pos.x + 5, pos.y + 5});
    } else {
        sfRectangleShape_setSize(bars[1], (sfVector2f) {5 * -percent, 50});
        sfRectangleShape_setPosition(bars[1],
            (sfVector2f) {pos.x - (5 * -percent) + 5, pos.y + 5});
    }
    print_string_progression(window, percent, bars);
}
