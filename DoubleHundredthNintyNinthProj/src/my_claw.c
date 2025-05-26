/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** claw
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my.h"

void play_claw_music(void)
{
    static sfMusic *mus = NULL;
    if (mus == NULL)
        mus = sfMusic_createFromFile("assets/sounds/claw.ogg");
    sfMusic_setPlayingOffset(mus, (sfTime) {0});
    sfMusic_play(mus);
}

void claw_sprite(rpg_t *rpg)
{
    static int claw_frame = 0;
    static sfClock *claw_clock = NULL;
    sfIntRect rect = {0, 0, 0, 0};

    if (!claw_clock)
        claw_clock = sfClock_create();
    if (sfTime_asMilliseconds(sfClock_getElapsedTime(claw_clock)) >= 100000) {
        sfClock_restart(claw_clock);
        claw_frame++;
        if (claw_frame >= 8)
            claw_frame = 0;
    }
    rect = (sfIntRect) {claw_frame * 0, 0, 192, 192};
    sfSprite_setTextureRect(rpg->character->sprite_claw, rect);
}

void destroy_music_and_claw(void)
{
    static sfClock *claw_clock = NULL;
    static sfMusic *mus = NULL;

    sfMusic_destroy(mus);
    mus = NULL;
    sfClock_destroy(claw_clock);
    claw_clock = NULL;
}
