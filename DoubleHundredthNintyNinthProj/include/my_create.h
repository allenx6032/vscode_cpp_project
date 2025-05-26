/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** create header
*/

#ifndef CREATE_H_
    #define CREATE_H_

    #include "my_typedef.h"

void setup_sprites(rpg_t *rpg);
void setup_maps(rpg_t *rpg);
void setup_sounds(rpg_t *rpg);
void setup_claw(rpg_t *rpg);
void setup_health(rpg_t *rpg);
void setup_monaco(character_t *character);
rpg_t *create_rpg(void);
cats_t *create_cats(void);
char_info_t *create_perso(bool load_save, char **env);
char_info_t *load(char **env);
void create_ennemi_health(rpg_t *rpg);
void setup_claw2(rpg_t *rpg);
ennemi_t setup_ennemi_t(rpg_t *rpg, enum cat_names name);

#endif
