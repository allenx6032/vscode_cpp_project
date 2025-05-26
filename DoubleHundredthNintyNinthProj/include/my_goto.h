/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** goto
*/

#ifndef GOTO_H_
    #define GOTO_H_

    #include "my_typedef.h"

void goto_house1(rpg_t *rpg);
void goto_map1(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_map2(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_inn(rpg_t *rpg);
void goto_bar(rpg_t *rpg);
void goto_weapon(rpg_t *rpg);
void goto_armor(rpg_t *rpg);
void goto_swamp(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_crack_house_red(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_crack_house_gray(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_potion_house(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);
void goto_tp_room(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char);

#endif
