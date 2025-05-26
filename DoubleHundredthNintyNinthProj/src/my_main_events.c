/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main_events
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"
#include "my_defines.h"

void misc_events(rpg_t *rpg)
{
    moving_cases(rpg);
    red_collision(rpg);
    green_collision(rpg);
    yellow_collision(rpg);
    blue_collision(rpg);
    purple_collision(rpg);
    grey_collision(rpg);
    orange_collision(rpg);
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++)
        set_circle(rpg, i);
    update_map_position(rpg, rpg->character->perso->id_zone);
    sfRenderWindow_clear(rpg->window, sfBlack);
    sfRenderWindow_drawSprite(rpg->window,
        rpg->menu->map->sprite[rpg->character->perso->id_zone], NULL);
    sfRenderWindow_drawSprite(rpg->window, rpg->character->sprite, NULL);
    sfRenderWindow_drawCircleShape(rpg->window, rpg->character->hitbox, NULL);
}

void frees(rpg_t *rpg)
{
    free(rpg->character->cats);
    free(rpg->character);
    free(rpg->menu->map);
    free(rpg->menu);
    free(rpg);
}
