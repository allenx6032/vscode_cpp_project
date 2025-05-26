/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** maps
*/

#include "my.h"
#include "my_var.h"
#include "my_utils.h"
#include "my_defines.h"

void setup_maps(rpg_t *rpg)
{
    rpg->menu->map->sprite = (sfSprite **)
        malloc_empty(sizeof(sfSprite *) * MAP_NAMES_SIZE);
    rpg->menu->map->texture = (sfTexture **)
        malloc_empty(sizeof(sfTexture *) * MAP_NAMES_SIZE);
    rpg->menu->map->image = (sfImage **)
        malloc_empty(sizeof(sfImage *) * MAP_NAMES_SIZE);
    for (unsigned int i = 0; i < MAP_NAMES_SIZE; i++) {
        rpg->menu->map->sprite[i] = sfSprite_create();
        rpg->menu->map->texture[i] =
            sfTexture_createFromFile(path_map_texture[i], NULL);
        sfSprite_setTexture(rpg->menu->map->sprite[i],
            rpg->menu->map->texture[i], sfTrue);
        sfSprite_setScale(rpg->menu->map->sprite[i],
            (sfVector2f){MAP_SCALE, MAP_SCALE});
        rpg->menu->map->image[i] = sfTexture_copyToImage(
            rpg->menu->map->texture[i]);
    }
}
