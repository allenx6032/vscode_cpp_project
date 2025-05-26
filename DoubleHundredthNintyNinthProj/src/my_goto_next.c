/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** goto_next
*/

#include "my.h"
#include "my_positions.h"
#include "my_defines.h"
#include "my_var.h"
#include "my_utils.h"

void goto_crack_house_red(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = CRACK_HOUSE_RED;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[CRACK_HOUSE_RED], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[CRACK_HOUSE_RED_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}

void goto_crack_house_gray(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = CRACK_HOUSE_GRAY;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[CRACK_HOUSE_GRAY], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite
        [CRACK_HOUSE_GRAY_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}

void goto_potion_house(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = POTION;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[POTION], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[POTION_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}

void goto_tp_room(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = TP_ROOM;
    for (int i = 0; i != CAT_NAMES_SIZE; i++) {
        rpg->character->cats->is_alive[i] = false;
        rpg->character->cats->health[i] = ennemi_health[i];
    }
    sfSprite_setPosition(rpg->menu->map->sprite[TP_ROOM], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[TP_ROOM_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}

void goto_swamp(rpg_t *rpg, sfVector2f pos_map, sfVector2f pos_char)
{
    unsigned int i = get_randint(0, CAT_NAMES_SIZE - 1);

    rpg->character->perso->old_zone = rpg->character->perso->id_zone;
    rpg->character->perso->id_zone = SWAMP;
    for (unsigned int j = 0; j != CAT_NAMES_SIZE; j++) {
        rpg->character->cats->is_alive[j] = false;
        rpg->character->cats->health[j] = ennemi_health[j];
    }
    rpg->character->cats->is_alive[i] = true;
    rpg->character->cats->health[i] = ennemi_health[i];
    sfCircleShape_setRadius(rpg->character->cats->hitbox[i], 200);
    sfSprite_setPosition(rpg->character->cats->sprites[i],
        (sfVector2f) {pos_char.x, pos_char.y - 600});
    sfSprite_setPosition(rpg->menu->map->sprite[SWAMP], pos_map);
    sfSprite_setPosition(rpg->menu->map->sprite[SWAMP_PATH], pos_map);
    sfSprite_setPosition(rpg->character->sprite, pos_char);
    sfCircleShape_setPosition(rpg->character->hitbox, pos_char);
}
