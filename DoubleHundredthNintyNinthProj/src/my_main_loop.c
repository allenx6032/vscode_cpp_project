/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main_loop
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"
#include "my_defines.h"
#include "my_create.h"
#include "my_destroy.h"

void key_l_pressed(rpg_t *rpg, char **env)
{
    if (rpg->event.type == sfEvtKeyPressed && rpg->event.key.code == sfKeyL) {
        destroy_perso(rpg->character->perso);
        rpg->character->perso = create_perso(true, env);
        sfSprite_setPosition(rpg->character->sprite, (sfVector2f)
            {rpg->character->perso->pos_x, rpg->character->perso->pos_y});
        sfSprite_setPosition(rpg->menu->map->sprite
            [rpg->character->perso->id_zone], (sfVector2f)
            {rpg->character->perso->map_pos_x,
            rpg->character->perso->map_pos_y});
        sfSprite_setPosition(rpg->menu->map->sprite
            [rpg->character->perso->id_zone + 1], (sfVector2f)
            {rpg->character->perso->map_pos_x,
            rpg->character->perso->map_pos_y});
    }
}

void keys_pressed(rpg_t *rpg, char **env)
{
    if (rpg->event.type == sfEvtKeyPressed &&
        (rpg->event.key.code == sfKeyLShift))
        rpg->character->speed = CHAR_SPEED_FAST;
    if (rpg->event.type == sfEvtKeyReleased &&
        rpg->event.key.code == sfKeyLShift)
        rpg->character->speed = CHAR_SPEED_DEFAULT;
    if (rpg->event.type == sfEvtKeyPressed && rpg->event.key.code == sfKeyE)
        pink_collision(rpg);
    if (rpg->event.type == sfEvtKeyPressed && rpg->event.key.code == sfKeyK) {
        rpg->character->perso->pos_x = sfSprite_getPosition
        (rpg->character->sprite).x;
        rpg->character->perso->pos_y = sfSprite_getPosition
        (rpg->character->sprite).y;
        rpg->character->perso->map_pos_x = sfSprite_getPosition
        (rpg->menu->map->sprite[rpg->character->perso->id_zone]).x;
        rpg->character->perso->map_pos_y = sfSprite_getPosition
        (rpg->menu->map->sprite[rpg->character->perso->id_zone]).y;
        save(rpg->character->perso, env);
    }
    key_l_pressed(rpg, env);
}

void second_loop(rpg_t *rpg)
{
    if (rpg->character->perso->is_ennemi_attacking == true) {
        ennemy_attacking(rpg);
    } else if (rpg->character->perso->is_attacking) {
        perso_attack(rpg);
    }
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        if (rpg->character->cats->is_alive[i])
            sfRenderWindow_drawSprite(rpg->window, rpg->character->cats->sprites
                [i], NULL);
    }
    draw_health(rpg);
    sfRenderWindow_drawSprite(rpg->window, rpg->character->monaco_spr, NULL);
    sfRenderWindow_display(rpg->window);
}

void loop(rpg_t *rpg, char **env)
{
    while (sfRenderWindow_pollEvent(rpg->window, &rpg->event)) {
        if (rpg->event.type == sfEvtClosed ||
            (rpg->event.type == sfEvtKeyPressed
            && rpg->event.key.code == sfKeyEscape))
            sfRenderWindow_close(rpg->window);
        focus_test(rpg);
        play_meow(rpg);
        keys_pressed(rpg, env);
    }
    misc_events(rpg);
    second_loop(rpg);
}
