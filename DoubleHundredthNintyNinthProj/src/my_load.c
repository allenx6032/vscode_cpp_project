/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** load (returns perso on success and NULL on failure)
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_save_only.h"
#include "my_utils.h"
#include "my.h"
#include "my_defines.h"
#include "my_destroy.h"
#include "my_inventory.h"

static char_info_t *malloc_persos(char_info_t *perso, FILE *file)
{
    char_info_t *perso_err = malloc_empty(sizeof(char_info_t));

    if (perso_err == NULL)
        return NULL;
    fread(&(perso->inventory_size), sizeof(size_t), 1, file);
    perso->inventory = malloc_empty(sizeof(items_t) * perso->inventory_size);
    perso->stats = malloc_empty(sizeof(statistics_t));
    perso_err->inventory = malloc_empty(sizeof(items_t) *
        perso->inventory_size);
    perso_err->stats = malloc_empty(sizeof(statistics_t));
    perso->quest = malloc_empty(sizeof(enum quest_status) * QUEST_NB);
    perso_err->quest = malloc_empty(sizeof(enum quest_status) * QUEST_NB);
    if (perso_err->inventory == NULL || perso_err->stats == NULL || perso->stats
        == NULL || perso->inventory == NULL || perso->quest == NULL ||
        perso_err->quest == NULL) {
        fclose(file);
        destroy_perso(perso_err);
        return NULL;
    }
    return perso_err;
}

static void read_fill_perso_part_2(char_info_t *perso,
    char_info_t *perso_err, FILE *file)
{
    fread(&(perso_err->id_zone), sizeof(unsigned int), 1, file);
    fread(&(perso->old_zone), sizeof(unsigned int), 1, file);
    fread(&(perso_err->old_zone), sizeof(unsigned int), 1, file);
    fread(&(perso->pos_x), sizeof(unsigned int), 1, file);
    fread(&(perso->map_pos_x), sizeof(float), 1, file);
    fread(perso->inventory, sizeof(items_t), perso->inventory_size, file);
    fclose(file);
}

static void read_fill_perso(char_info_t *perso,
    char_info_t *perso_err, FILE *file)
{
    fread(&(perso_err->pos_x), sizeof(unsigned int), 1, file);
    fread(&(perso_err->map_pos_y), sizeof(float), 1, file);
    fread(perso_err->inventory, sizeof(items_t),
        perso->inventory_size, file);
    fread(&(perso->id_zone), sizeof(unsigned int), 1, file);
    fread(&(perso->map_pos_y), sizeof(float), 1, file);
    fread(&(perso_err->inventory_size), sizeof(size_t), 1, file);
    fread(perso->stats, sizeof(statistics_t), 1, file);
    fread(perso_err->stats, sizeof(statistics_t), 1, file);
    fread(perso->quest, sizeof(enum quest_status) * QUEST_NB, 1, file);
    fread(&(perso_err->pos_y), sizeof(unsigned int), 1, file);
    fread(perso_err->quest, sizeof(enum quest_status) * QUEST_NB, 1, file);
    fread(&(perso_err->map_pos_x), sizeof(float), 1, file);
    fread(&(perso->pos_y), sizeof(unsigned int), 1, file);
    read_fill_perso_part_2(perso, perso_err, file);
}

static bool tests_malloc(FILE *file, char_info_t *perso)
{
    bool test = false;

    if (file == NULL || perso == NULL) {
        test = true;
    }
    if (test && file != NULL)
        fclose(file);
    if (test)
        free(perso);
    return test;
}

char_info_t *load(char **env)
{
    char_info_t *perso = malloc_empty(sizeof(char_info_t));
    char_info_t *perso_err;
    FILE *file = get_save(env);

    if (tests_malloc(file, perso))
        return NULL;
    perso_err = malloc_persos(perso, file);
    if (perso_err == NULL) {
        destroy_perso(perso);
        return NULL;
    }
    read_fill_perso(perso, perso_err, file);
    test_corruption(perso, perso_err);
    destroy_perso(perso_err);
    return perso;
}
