/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** collision_switch
*/

#include "my.h"
#include "my_goto.h"
#include "my_loadings.h"
#include "my_positions.h"

void cases_red_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->old_zone) {
        case MAP_1:
            small_loading(rpg->window);
            goto_map1(rpg, MAP1_POS_MAP_RED, MAP1_POS_CHAR_RED);
            break;
        case MAP_2:
            small_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_RED, MAP2_POS_CHAR_RED);
            break;
        default:
            break;
    }
}

void cases_green_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->old_zone) {
        case MAP_1:
            small_loading(rpg->window);
            goto_map1(rpg, MAP1_POS_MAP_GREEN, MAP1_POS_CHAR_GREEN);
            break;
        case MAP_2:
            small_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_GREEN, MAP2_POS_CHAR_GREEN);
            break;
        default:
            break;
    }
}

void cases_pink_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case BAR:
            serve_monaco(rpg);
            break;
        case INN:
            quest_1(rpg);
            break;
        case POTION:
            catnip(rpg);
            break;
        default:
            break;
    }
}

void cases_yellow_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->old_zone) {
        case MAP_1:
            small_loading(rpg->window);
            goto_map1(rpg, MAP1_POS_MAP_YELLOW, MAP1_POS_CHAR_YELLOW);
            break;
        case MAP_2:
            small_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_YELLOW, MAP2_POS_CHAR_YELLOW);
            break;
        default:
            break;
    }
}

void cases_blue_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case MAP_1:
            medium_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP, MAP2_POS_CHAR);
            break;
        case MAP_2:
            medium_loading(rpg->window);
            goto_swamp(rpg, SWAMP_POS_MAP_BOTTOM, SWAMP_POS_CHAR_BOTTOM);
            break;
        case SWAMP:
            medium_loading(rpg->window);
            goto_tp_room(rpg, TP_ROOM_POS_MAP, TP_ROOM_POS_CHAR);
            break;
        default:
            break;
    }
}
