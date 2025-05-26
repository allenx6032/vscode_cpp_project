/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** destroy
*/

#include <stdlib.h>
#include "my.h"
#include "my_defines.h"
#include "my_destroy.h"

void destroy_cats(cats_t *cats)
{
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        sfCircleShape_destroy(cats->hitbox[i]);
        sfSprite_destroy(cats->sprites[i]);
        sfTexture_destroy(cats->textures[i]);
    }
    free(cats->sprites);
    free(cats->textures);
    free(cats->hitbox);
    free(cats);
}

void destroy_perso(char_info_t *perso)
{
    free(perso->quest);
    free(perso->inventory);
    free(perso->stats);
    free(perso);
}

static void destroy_random_objects(rpg_t *rpg)
{
    if (rpg->character->monaco_spr) {
        sfSprite_destroy(rpg->character->monaco_spr);
        sfTexture_destroy(rpg->character->monaco_tex);
    }
    if (rpg->character->sprite_claw) {
        sfSprite_destroy(rpg->character->sprite_claw);
        sfTexture_destroy(rpg->character->texture_claw);
    }
    if (rpg->character->sprite_red_claw) {
        sfSprite_destroy(rpg->character->sprite_red_claw);
        sfTexture_destroy(rpg->character->texture_red_claw);
    }
    if (rpg->character->clock)
        sfClock_destroy(rpg->character->clock);
    if (rpg->character->clock2)
        sfClock_destroy(rpg->character->clock2);
    if (rpg->character->clock3)
        sfClock_destroy(rpg->character->clock3);
}

void destroy_rpg(rpg_t *rpg)
{
    destroy_random_objects(rpg);
    sfMusic_destroy(rpg->character->music);
    sfSound_destroy(rpg->character->meow);
    sfRectangleShape_destroy(rpg->character->out_health);
    sfRectangleShape_destroy(rpg->character->health);
    sfSoundBuffer_destroy(rpg->character->cat_sound);
    destroy_cats(rpg->character->cats);
    sfCircleShape_destroy(rpg->character->hitbox);
    sfSprite_destroy(rpg->character->sprite);
    sfTexture_destroy(rpg->character->texture);
    destroy_perso(rpg->character->perso);
    destroy_ennemy_clock();
    destroy_music_and_claw();
    free(rpg->menu->map);
    free(rpg->menu);
    sfRenderWindow_destroy(rpg->window);
}
