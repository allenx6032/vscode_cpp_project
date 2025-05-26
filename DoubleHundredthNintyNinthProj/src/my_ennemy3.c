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
#include "my_defines.h"
#include "my_typedef.h"
#include "my_var.h"


void ennemy_attack(rpg_t *rpg, enum cat_names name)
{
    static sfClock *last_attack_clock_perso = NULL;
    sfVector2f pos1_center = sfCircleShape_getPosition(rpg->character->hitbox);
    sfVector2f pos2_center = sfCircleShape_getPosition(
        rpg->character->cats->hitbox[name]);
    float dist = 0;

    pos1_center.x += sfCircleShape_getRadius(rpg->character->hitbox);
    pos1_center.y += sfCircleShape_getRadius(rpg->character->hitbox);
    pos2_center.x += sfCircleShape_getRadius(
        rpg->character->cats->hitbox[name]);
    pos2_center.y += sfCircleShape_getRadius(
        rpg->character->cats->hitbox[name]);
    dist = sqrt(pow(pos1_center.x - pos2_center.x, 2) +
        pow(pos1_center.y - pos2_center.y, 2));
    if (!last_attack_clock_perso) {
        last_attack_clock_perso = sfClock_create();
    }
    ennemy_attack2(rpg, dist, last_attack_clock_perso, name);
}

void ennemy_attack2(rpg_t *rpg, float dist, sfClock* last_attack_clock_perso,
    enum cat_names name)
{
    static sfClock *last_attack_clock_ennemy = NULL;
    sfTime elapsed_perso = sfClock_getElapsedTime(last_attack_clock_perso);
    sfTime elapsed_ennemy = {0};
    if (last_attack_clock_ennemy != NULL)
        elapsed_ennemy = sfClock_getElapsedTime(last_attack_clock_ennemy);
    if (dist <= ATTACK_PLAYER_DISTANCE && sfMouse_isButtonPressed
        (sfMouseLeft) && sfTime_asSeconds(elapsed_perso) >= 1.5f) {
        rpg->character->cats->health[name] -=
        OVER(rpg->character->perso->stats->strenth, ennemi_defense[name]);
        rpg->character->perso->is_attacking = true;
        sfClock_restart(last_attack_clock_perso);
    }
    if (!last_attack_clock_ennemy)
        last_attack_clock_ennemy = sfClock_create();
    if (dist <= ATTACK_ENNEMY_DISTANCE &&
        sfTime_asSeconds(elapsed_ennemy) >= 1.5f) {
        for_ennemy_attack2(rpg, name);
        play_claw_music();
        sfClock_restart(last_attack_clock_ennemy);
    }
}

void for_ennemy_attack2(rpg_t *rpg, enum cat_names name)
{
    rpg->character->perso->stats->hp -= OVER(ennemi_strenth[name],
        rpg->character->perso->stats->defense);
    rpg->character->perso->is_ennemi_attacking = true;
}
