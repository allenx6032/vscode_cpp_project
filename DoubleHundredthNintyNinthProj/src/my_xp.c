/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** xp
*/

#include "my_var.h"
#include "my_typedef.h"
#include "my_utils.h"
#include "my_defines.h"
#include "my.h"

static void do_a_level_up(rpg_t *rpg)
{
    int randin_hp = get_randint(5, 10);

    if (rpg->character->perso->stats->lvl >= 50)
        return;
    print_text(rpg, LVL_UP_START, LVL_UP_END, NOONE);
    rpg->character->perso->stats->xp -=
        rpg->character->perso->stats->xp_to_next_level;
    rpg->character->perso->stats->lvl += 1;
    rpg->character->perso->stats->defense += get_randint(1, 5);
    rpg->character->perso->stats->strenth += get_randint(3, 10);
    rpg->character->perso->stats->hp_max += randin_hp;
    rpg->character->perso->stats->hp += randin_hp;
    rpg->character->perso->stats->xp_to_next_level =
        xp_needed[rpg->character->perso->stats->lvl];
    if (rpg->character->perso->stats->lvl >= 50)
        print_text(rpg, LVL_MAX_START, LVL_MAX_END, NOONE);
}

void add_xp(rpg_t *rpg, enum cat_names name)
{
    int xp_to_add = get_randint(xp_min[name], xp_max[name]);

    if ((int) (rpg->character->perso->stats->xp + xp_to_add) < 0)
        rpg->character->perso->stats->xp = 0;
    else
        rpg->character->perso->stats->xp += xp_to_add;
    if (XP_TEST_(rpg->character->perso->stats))
        do_a_level_up(rpg);
}
