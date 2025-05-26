/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main
*/

#include <stdlib.h>
#include "my.h"
#include "my_defines.h"
#include "my_utils.h"
#include "my_inventory.h"

int main(int ac, char **av, char **env)
{
    char_info_t *perso = malloc_empty(sizeof(char_info_t));
    perso->inventory = malloc_empty(sizeof(items_t) * 5);
    perso->stats = malloc_empty(sizeof(statistics_t));
    perso->quest = malloc_empty(sizeof(unsigned int) * QUEST_NB);
    perso->inventory_size = 5;
    perso->id_zone = 3;
    perso->pos_x = 4;
    perso->stats->hp = 40;
    perso->stats->hp_max = 20;
    perso->inventory[3].name = HELMET;
    perso->inventory[3].can_be_equiped = true;
    perso->inventory[3].is_equiped = false;
    perso->quest[0] = KNOWN;
    perso->pos_y = 4;
    save(perso, env);
    free(perso->stats);
    free(perso->inventory);
    free(perso);
}
