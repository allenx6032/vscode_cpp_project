/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** text_loop
*/

#include "my.h"
#include "my_text.h"
#include "my_defines.h"

static unsigned int next_text(text_t *text, unsigned int i)
{
    if (i >= text->size)
        text->test = true;
    else {
        while (i < text->size) {
            sfClock_restart(text->clock);
            text->displayed_message[i] = (*text->message)[i];
            text->displayed_message[i + 1] = '\0';
            i++;
        }
    }
    return i;
}

static unsigned int events(text_t *text, rpg_t *rpg, unsigned int i)
{
    sfEvent tmp;

    while (sfRenderWindow_pollEvent(rpg->window, &text->event)) {
        if (text->event.type == sfEvtClosed || (text->event.type ==
            sfEvtKeyPressed && text->event.key.code == sfKeyEscape))
            sfRenderWindow_close(rpg->window);
        tmp = rpg->event;
        rpg->event = text->event;
        focus_test(rpg);
        rpg->event = tmp;
        if (((text->event.type == sfEvtKeyPressed) && (text->event.key.code ==
            sfKeySpace || text->event.key.code == sfKeyEnter))
            || (text->event.type == sfEvtMouseButtonPressed &&
            text->event.mouseButton.button == sfMouseLeft)) {
            i = next_text(text, i);
        }
    }
    return i;
}

static void display(text_t *text, rpg_t *rpg)
{
    sfRenderWindow_clear(rpg->window, sfBlack);
    sfRenderWindow_drawSprite(rpg->window,
        rpg->menu->map->sprite[rpg->character->perso->id_zone], NULL);
    sfRenderWindow_drawSprite(rpg->window, rpg->character->sprite, NULL);
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        if (rpg->character->cats->is_alive[i])
            sfRenderWindow_drawSprite(rpg->window, rpg->character->cats->sprites
                [i], NULL);
    }
    sfRenderWindow_drawRectangleShape(rpg->window,
        rpg->character->out_health, NULL);
    sfRenderWindow_drawRectangleShape(rpg->window, rpg->character->health,
        NULL);
    if (text->back_ground_spr)
        sfRenderWindow_drawSprite(rpg->window, text->back_ground_spr, NULL);
    else
        sfRenderWindow_drawRectangleShape(rpg->window, text->back_ground_rect,
            NULL);
    sfRenderWindow_drawText(rpg->window, text->text, NULL);
    sfRenderWindow_display(rpg->window);
}

unsigned int do_the_loop_in_text(text_t *text, rpg_t *rpg, unsigned int i)
{
    i = events(text, rpg, i);
    if (i < text->size && sfTime_asMilliseconds(sfClock_getElapsedTime
        (text->clock)) > 60) {
        sfClock_restart(text->clock);
        text->displayed_message[i] = (*text->message)[i];
        text->displayed_message[i + 1] = '\0';
        i++;
    }
    sfText_setString(text->text, text->displayed_message);
    display(text, rpg);
    return i;
}
