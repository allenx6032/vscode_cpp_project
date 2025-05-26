/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main
*/

#include <stdlib.h>
#include <stdio.h>
#include "my.h"
#include "my_create.h"
#include "my_defines.h"
#include "my_utils.h"
#include "my_inventory.h"

static void main_2(char_info_t *perso)
{
    printf( "taille inv:%d\n", perso->inventory_size);
    printf( "zone:%d\n", perso->id_zone);
    printf( "pos x:%d\n", perso->pos_x);
    printf( "pos y:%d\n", perso->pos_y);
    printf( "stats:\n");
    printf( "\tdef:%d\n", perso->stats->defense);
    printf( "\tstr:%d\n", perso->stats->strenth);
    printf( "\thp:%d/", perso->stats->hp);
    printf( "%d\n", perso->stats->hp_max);
    printf( "\tlvl:%d\n", perso->stats->lvl);
    printf( "\txp:%d", perso->stats->xp);
    printf( "/%d\n", perso->stats->xp_to_next_level);
    printf( "inv :");
}

int main(int ac, char **av, char **env)
{
    char_info_t *perso = load(env);

    if (perso == NULL)
        return 0;
    for (unsigned int i = 0; i < perso->inventory_size; i++) {
        printf( "\tname : %d, can_equi : %d, is_equi : %d\n",
            perso->inventory[i].name, perso->inventory[i].can_be_equiped,
            perso->inventory[i].is_equiped);
    }
    for (unsigned int i = 0; i < QUEST_NB; i++) {
        printf( "Quest %u: state %u\n", i + 1, perso->quest[i]);
    }
    free(perso->quest);
    free(perso->stats);
    free(perso->inventory);
    free(perso);
}
