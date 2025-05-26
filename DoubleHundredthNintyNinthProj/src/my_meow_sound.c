/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** sounds
*/

#include "my.h"
#include "my_utils.h"

void play_meow(rpg_t *rpg)
{
    if (rpg->event.type == sfEvtKeyPressed &&
    (rpg->event.key.code == sfKeyC)) {
        sfSound_play(rpg->character->meow);
    }
}
