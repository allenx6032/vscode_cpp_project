/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** focus
*/

#include "my.h"

void focus_test(rpg_t *rpg)
{
    if (rpg->event.type != sfEvtLostFocus)
        return;
    sfMusic_pause(rpg->character->music);
    while (rpg->event.type != sfEvtGainedFocus) {
        sfRenderWindow_pollEvent(rpg->window, &rpg->event);
        if (rpg->event.type == sfEvtClosed) {
            sfRenderWindow_close(rpg->window);
        }
    }
    sfMusic_play(rpg->character->music);
}
