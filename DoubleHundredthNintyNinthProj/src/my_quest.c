/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** quests
*/

#include "my.h"
#include "my_inventory.h"

void quest_2(rpg_t *rpg)
{
    switch (rpg->character->perso->quest[QUEST_2]) {
        case UNKNOWN:
            print_text(rpg, QUEST_2_UNKNOWN_START, QUEST_2_UNKNOWN_END, BENJ);
            rpg->character->perso->quest[QUEST_2] = KNOWN;
            break;
        case KNOWN:
            print_text(rpg, QUEST_2_KNOWN_START, QUEST_2_KNOWN_END, BENJ);
            break;
        case DONE:
            remove_item(rpg, CATNIP_QUEST);
            print_text(rpg, QUEST_2_DONE_START, QUEST_2_DONE_END, BENJ);
            rpg->character->perso->quest[QUEST_2] = ENDED;
            break;
        case ENDED:
            break;
        default:
            break;
    }
}

void quest_1(rpg_t *rpg)
{
    switch (rpg->character->perso->quest[QUEST_1]) {
        case UNKNOWN:
            print_text(rpg, QUEST_1_UNKNOWN_START, QUEST_1_UNKNOWN_END, BENJ);
            rpg->character->perso->quest[QUEST_1] = KNOWN;
            break;
        case KNOWN:
            print_text(rpg, QUEST_1_KNOWN_START, QUEST_1_KNOWN_END, BENJ);
            break;
        case DONE:
            remove_item(rpg, MONACO_QUEST);
            print_text(rpg, QUEST_1_DONE_START, QUEST_1_DONE_END,BENJ);
            rpg->character->perso->quest[QUEST_1] = ENDED;
            break;
        case ENDED:
            quest_2(rpg);
            break;
        default:
            break;
    }
}
