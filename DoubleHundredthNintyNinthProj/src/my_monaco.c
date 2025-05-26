/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** monaco
*/

#include "my.h"
#include "my_inventory.h"

void setup_monaco(character_t *character)
{
    character->monaco_spr = sfSprite_create();
    character->monaco_tex = sfTexture_createFromFile(
        "assets/sprites/misc/monaco.png", NULL);
    sfSprite_setTexture(character->monaco_spr, character->monaco_tex, sfTrue);
    sfSprite_setScale(character->monaco_spr, (sfVector2f){0, 0});
}

void map_move_monaco(sfVector2f monaco_move, rpg_t *rpg)
{
    sfVector2f pos = (sfVector2f) {0, 0};
    sfFloatRect size = (sfFloatRect) {0, 0, 0, 0};
    float radius = 0;

    pos = sfSprite_getPosition(rpg->character->monaco_spr);
    size = sfSprite_getGlobalBounds(rpg->character->monaco_spr);
    pos.x += monaco_move.x;
    pos.y += monaco_move.y;
    sfSprite_setPosition(rpg->character->monaco_spr, pos);
    pos.x -= radius;
    pos.y -= radius;
    pos.x += size.width / 2;
    pos.y += size.height / 2;
}

void serve_monaco(rpg_t *rpg)
{
    if (rpg->character->perso->quest[QUEST_1] == KNOWN) {
        if (!add_item(rpg, MONACO_QUEST, false))
            return;
        sfSprite_setScale(rpg->character->monaco_spr, (sfVector2f){0.2, 0.2});
        print_text(rpg, QUEST_1_GET_START, QUEST_1_GET_END, DAMIAN);
        rpg->character->perso->quest[QUEST_1] = DONE;
    }
}

void catnip(rpg_t *rpg)
{
    if (rpg->character->perso->quest[QUEST_2] == KNOWN) {
        if (!add_item(rpg, CATNIP_QUEST, false))
            return;
        rpg->character->perso->quest[QUEST_2] = DONE;
        print_text(rpg, QUEST_2_GET_START, QUEST_1_GET_END, MIKAEL);
    }
}
