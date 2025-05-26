/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** sprites
*/

#include "my.h"
#include "my_create.h"
#include "my_var.h"
#include "my_defines.h"

void setup_sounds(rpg_t *rpg)
{
    rpg->character->cat_sound =
        sfSoundBuffer_createFromFile("assets/sounds/meow.ogg");
    rpg->character->meow = sfSound_create();
    sfSound_setBuffer(rpg->character->meow,
    rpg->character->cat_sound);
}

void main_music(rpg_t *rpg)
{
    rpg->character->music = sfMusic_createFromFile
    ("assets/sounds/FKJ - Ylang Ylang.ogg");
    sfMusic_setLoop(rpg->character->music, sfTrue);
    sfMusic_play(rpg->character->music);
}
