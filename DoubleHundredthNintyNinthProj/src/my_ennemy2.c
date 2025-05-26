/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** ennemy
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
#include "my_create.h"
#include "my_destroy.h"
#include "my_typedef.h"
#include "my_var.h"

void destroy_ennemy_clock(void)
{
    static sfClock *last_attack_clock_perso = NULL;
    static sfClock *last_attack_clock_ennemy = NULL;

    if (last_attack_clock_ennemy) {
        sfClock_destroy(last_attack_clock_ennemy);
        last_attack_clock_ennemy = NULL;
    }
    if (last_attack_clock_perso) {
        sfClock_destroy(last_attack_clock_perso);
        last_attack_clock_perso = NULL;
    }
}

static void continue_ennemi(rpg_t *rpg, enum cat_names name)
{
    ennemy_attack(rpg, name);
    if (rpg->character->perso->stats->hp <= 0) {
        death(rpg);
    }
    if (rpg->character->cats->health[name] <= 0 &&
        rpg->character->cats->is_alive[name] == true) {
        add_xp(rpg, name);
        rpg->character->cats->is_alive[name] = false;
    }
}

void what_if_ennemy(rpg_t *rpg, sfVector2f movement, enum cat_names name)
{
    ennemi_t nemi = setup_ennemi_t(rpg, name);

    if (rpg->character->clock2 == NULL)
        rpg->character->clock2 = sfClock_create();
    if (sfTime_asMilliseconds(sfClock_getElapsedTime
        (rpg->character->clock2)) >= 1) {
        sfClock_restart(rpg->character->clock2);
        if (sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
            [rpg->character->perso->id_zone + 1], nemi.pixel_pos.x,
            nemi.pixel_pos.y)) != sfColor_toInteger(sfBlack) &&
            sfColor_toInteger(sfImage_getPixel(rpg->menu->map->image
            [rpg->character->perso->id_zone + 1], nemi.pixel_pos.x,
            nemi.pixel_pos.y)) != sfColor_toInteger(sfBlack)) {
        sfSprite_move(rpg->character->cats->sprites[name],
        movement);
        } else
            sfSprite_move(rpg->character->cats->sprites[name],
                (sfVector2f) {movement.x * -1, movement.y * -1});
    }
    continue_ennemi(rpg, name);
}
