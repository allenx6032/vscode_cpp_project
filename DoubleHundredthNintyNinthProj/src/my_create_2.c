/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** create part 2
*/

#include "my_create.h"
#include "my_defines.h"
#include "my_utils.h"
#include "my_var.h"
#include "my_destroy.h"
#include "my_inventory.h"

char_info_t *set_perso(char_info_t *perso)
{
    perso->inventory_size = 16;
    perso->inventory = malloc_empty(sizeof(items_t) * 16);
    perso->stats = malloc_empty(sizeof(statistics_t));
    perso->quest = malloc_empty(sizeof(enum quest_status) * QUEST_NB);
    return perso;
}

char_info_t *attribute_perso(char_info_t *perso)
{
    perso->stats->hp = 100;
    perso->stats->hp_max = 100;
    perso->stats->xp = 0;
    perso->stats->lvl = 1;
    perso->stats->strenth = 10;
    perso->stats->defense = 10;
    perso->stats->xp_to_next_level = xp_needed[1];
    return perso;
}

char_info_t *create_perso(bool load_save, char **env)
{
    char_info_t *perso = NULL;

    if (load_save)
        return load(env);
    perso = malloc_empty(sizeof(char_info_t));
    if (perso == NULL)
        return NULL;
    perso = set_perso(perso);
    if (perso->inventory == NULL || perso->stats == NULL ||
        perso->quest == NULL) {
        destroy_perso(perso);
        return NULL;
    }
    attribute_perso(perso);
    return perso;
}

void create_ennemi_health(rpg_t *rpg)
{
    rpg->character->cats->health = malloc_empty(sizeof(int) * CAT_NAMES_SIZE);
    rpg->character->cats->is_alive =
        malloc_empty(sizeof(bool) * CAT_NAMES_SIZE);
}
