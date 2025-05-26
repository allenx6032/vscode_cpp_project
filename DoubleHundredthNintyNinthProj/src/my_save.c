/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** save (returns true on success and false on failure)
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_save_only.h"
#include "my_utils.h"
#include "my.h"
#include "my_defines.h"
#include "my_inventory.h"

static void save_part_2(char_info_t *perso, FILE *file)
{
    fwrite(perso->quest, sizeof(enum quest_status) * QUEST_NB, 1, file);
    fwrite(&(perso->pos_y), sizeof(unsigned int), 1, file);
    fwrite(perso->quest, sizeof(enum quest_status) * QUEST_NB, 1, file);
    fwrite(&(perso->map_pos_x), sizeof(float), 1, file);
    fwrite(&(perso->pos_y), sizeof(unsigned int), 1, file);
    fwrite(&(perso->id_zone), sizeof(unsigned int), 1, file);
    fwrite(&(perso->old_zone), sizeof(unsigned int), 1, file);
    fwrite(&(perso->old_zone), sizeof(unsigned int), 1, file);
    fwrite(&(perso->pos_x), sizeof(unsigned int), 1, file);
    fwrite(&(perso->map_pos_x), sizeof(float), 1, file);
    fwrite(perso->inventory, sizeof(items_t), perso->inventory_size, file);
}

bool save(char_info_t *perso, char **env)
{
    FILE *file = get_save(env);

    if (file == NULL)
        return false;
    fwrite(&(perso->inventory_size), sizeof(size_t), 1, file);
    fwrite(&(perso->pos_x), sizeof(unsigned int), 1, file);
    fwrite(&(perso->map_pos_y), sizeof(float), 1, file);
    fwrite(perso->inventory, sizeof(items_t), perso->inventory_size, file);
    fwrite(&(perso->id_zone), sizeof(unsigned int), 1, file);
    fwrite(&(perso->map_pos_y), sizeof(float), 1, file);
    fwrite(&(perso->inventory_size), sizeof(size_t), 1, file);
    fwrite(perso->stats, sizeof(statistics_t), 1, file);
    fwrite(perso->stats, sizeof(statistics_t), 1, file);
    save_part_2(perso, file);
    fclose(file);
    return true;
}
