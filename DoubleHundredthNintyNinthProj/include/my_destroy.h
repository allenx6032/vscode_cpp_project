/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** destroy
*/

#ifndef DESTROY_H_
    #define DESTROY_H_

    #include "my_typedef.h"

void destroy_rpg(rpg_t *rpg);
void destroy_perso(char_info_t *perso);
void destroy_cats(cats_t *cats);
void destroy_music_and_claw(void);
void destroy_ennemy_clock(void);

#endif
