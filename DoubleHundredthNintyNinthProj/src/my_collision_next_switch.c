/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** collision_next_switch
*/

#include "my.h"
#include "my_goto.h"
#include "my_loadings.h"
#include "my_positions.h"

void cases_purple_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case MAP_2:
            medium_loading(rpg->window);
            goto_map1(rpg, MAP1_POS_MAP, MAP1_POS_CHAR);
            break;
        case SWAMP:
            medium_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_PURPLE, MAP2_POS_CHAR_PURPLE);
            break;
        case TP_ROOM:
            medium_loading(rpg->window);
            goto_swamp(rpg, SWAMP_POS_MAP_TOP, SWAMP_POS_CHAR_TOP);
            break;
        default:
            break;
    }
}

void cases_grey_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case MAP_2:
            small_loading(rpg->window);
            goto_crack_house_gray(rpg, CRACK_POS_MAP, CRACK_POS_CHAR);
            break;
        case CRACK_HOUSE_GRAY:
            small_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_GREY, MAP2_POS_CHAR_GREY);
            break;
        default:
            break;
    }
}

void cases_orange_coll(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case MAP_2:
            small_loading(rpg->window);
            goto_potion_house(rpg, POTION_POS_MAP, POTION_POS_CHAR);
            break;
        case POTION:
            small_loading(rpg->window);
            goto_map2(rpg, MAP2_POS_MAP_ORANGE, MAP2_POS_CHAR_ORANGE);
        default:
            break;
    }
}

void id_switch(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
    case HOUSE1:
        cases_red_coll(rpg);
        break;
    case MAP_1:
        small_loading(rpg->window);
        goto_house1(rpg);
        break;
    case MAP_2:
        small_loading(rpg->window);
        goto_crack_house_red(rpg, CRACK_POS_MAP, CRACK_POS_CHAR);
        break;
    case CRACK_HOUSE_RED:
        small_loading(rpg->window);
        goto_map2(rpg, MAP2_POS_MAP_RED, MAP2_POS_CHAR_RED);
        break;
    default:
        break;
    }
}

void green_switch(rpg_t *rpg)
{
    switch (rpg->character->perso->id_zone) {
        case INN:
            cases_green_coll(rpg);
            break;
        case WEAPON:
            cases_green_coll(rpg);
            break;
        case MAP_1:
            small_loading(rpg->window);
            goto_inn(rpg);
            break;
        case MAP_2:
            small_loading(rpg->window);
            goto_weapon(rpg);
            break;
    }
}
