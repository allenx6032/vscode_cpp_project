/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main_loop
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my_defines.h"
#include "my.h"


void second_if(rpg_t *rpg)
{
    sfFloatRect character_red_Bounds = sfSprite_getGlobalBounds
    (rpg->character->sprite);
    sfSprite_setPosition(rpg->character->sprite_red_claw,
    (sfVector2f){character_red_Bounds.left + character_red_Bounds.width / 5.0f,
    character_red_Bounds.top - sfSprite_getGlobalBounds
    (rpg->character->sprite_red_claw).height / 5.0f});
    sfRenderWindow_drawSprite
    (rpg->window, rpg->character->sprite_red_claw, NULL);
}

void ennemy_attacking(rpg_t *rpg)
{
    if (rpg->character->clock3 == NULL) {
        rpg->character->clock3 = sfClock_create();
    }
    if (sfTime_asMicroseconds(sfClock_getElapsedTime(
        rpg->character->clock3)) >= 100000) {
        sfClock_restart(rpg->character->clock3);
        sfIntRect rect2 = sfSprite_getTextureRect
        (rpg->character->sprite_red_claw);
        rect2.left += rect2.width;
        if (rect2.left >= 4 * rect2.width) {
            rect2.left = 0;
            rpg->character->perso->is_ennemi_attacking = false;
        }
        sfSprite_setTextureRect(rpg->character->sprite_red_claw, rect2);
    } else
        second_if(rpg);
}

void perso_attack(rpg_t *rpg)
{
    if (rpg->character->clock == NULL)
        rpg->character->clock = sfClock_create();
    if (sfTime_asMicroseconds(sfClock_getElapsedTime(rpg->character->clock))
        >= 50000) {
        sfClock_restart(rpg->character->clock);
        sfIntRect rect = sfSprite_getTextureRect(rpg->character->sprite_claw);
        rect.left += rect.width;
        if (rect.left >= 4 * rect.width) {
            rect.left = 0;
            rpg->character->perso->is_attacking = false;
        }
        sfSprite_setTextureRect(rpg->character->sprite_claw, rect);
    } else {
        third_if(rpg);
    }
}

void third_if(rpg_t *rpg)
{
    sfFloatRect characterBounds;

    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        if (rpg->character->cats->is_alive[i]) {
            characterBounds =
                sfSprite_getGlobalBounds(rpg->character->cats->sprites[i]);
            sfSprite_setPosition(rpg->character->sprite_claw, (sfVector2f)
                {characterBounds.left + characterBounds.width / 5.0f,
                characterBounds.top - sfSprite_getGlobalBounds(
                rpg->character->sprite_claw).height / 5.0f});
            sfRenderWindow_drawSprite(rpg->window, rpg->character->sprite_claw,
                NULL);
        }
    }
}
