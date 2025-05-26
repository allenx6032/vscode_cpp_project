/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** text_calls
*/

#include "my.h"

void print_text(rpg_t *rpg, enum text start, enum text end,
    enum speach_box who_speak)
{
    for (unsigned int i = start; i <= end; i++) {
        text_box(rpg, i, who_speak);
    }
}
