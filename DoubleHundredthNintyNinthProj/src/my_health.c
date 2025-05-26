/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** health
*/

#include "my.h"
#include "my_utils.h"
#include "my_var.h"

void setup_health(rpg_t *rpg)
{
    sfVector2u window_size = sfRenderWindow_getSize(rpg->window);
    sfVector2f bar_size = (sfVector2f) {500, 60};
    rpg->character->health = sfRectangleShape_create();
    rpg->character->out_health = sfRectangleShape_create();
    sfRectangleShape_setFillColor(rpg->character->health, sfRed);
    sfRectangleShape_setFillColor(rpg->character->out_health, sfWhite);
    sfRectangleShape_setOutlineColor(rpg->character->health, sfRed);
    sfRectangleShape_setOutlineColor(rpg->character->out_health, sfWhite);
    sfRectangleShape_setPosition(rpg->character->health, (sfVector2f) {50,
        window_size.y - 50 - bar_size.y});
    sfRectangleShape_setPosition(rpg->character->out_health, (sfVector2f) {40,
        window_size.y - 60 - bar_size.y});
    sfRectangleShape_setSize(rpg->character->health, (sfVector2f) {bar_size.x -
        20, bar_size.y - 20});
    sfRectangleShape_setSize(rpg->character->out_health, bar_size);
}

void draw_health(rpg_t *rpg)
{
    sfVector2f bar_size = (sfVector2f) {500, 60};

    bar_size = (sfVector2f) {bar_size.x - 20, bar_size.y - 20};
    bar_size.x /= rpg->character->perso->stats->hp_max;
    bar_size.x *= rpg->character->perso->stats->hp;
    sfRectangleShape_setSize(rpg->character->health, bar_size);
    sfRenderWindow_drawRectangleShape(rpg->window, rpg->character->out_health,
        NULL);
    sfRenderWindow_drawRectangleShape(rpg->window, rpg->character->health,
        NULL);
}
