/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** rpg
*/

#include "my.h"
#include "my_create.h"
#include "my_utils.h"
#include "my_defines.h"
#include "my_destroy.h"

static rpg_t *malloc_rpg_struct(rpg_t *rpg)
{
    rpg->menu = malloc_empty(sizeof(menu_t));
    rpg->character = malloc_empty(sizeof(character_t));
    if (rpg->character == NULL || rpg->menu == NULL) {
        destroy_rpg(rpg);
        return NULL;
    }
    rpg->character->speed = CHAR_SPEED_DEFAULT;
    rpg->menu->map = malloc_empty(sizeof(map_t));
    rpg->character->cats = create_cats();
    if (rpg->character->cats == NULL || rpg->menu->map == NULL) {
        destroy_rpg(rpg);
        return NULL;
    }
    return rpg;
}

rpg_t *create_rpg(void)
{
    rpg_t *rpg = malloc_empty(sizeof(rpg_t));

    if (rpg == NULL)
        return NULL;
    if (malloc_rpg_struct(rpg) == NULL)
        return NULL;
    rpg->window = sfRenderWindow_create((sfVideoMode) {1920, 1080, 32},
        "Catventure: A Feline Odyssey", sfDefaultStyle, NULL);
    sfRenderWindow_setFramerateLimit(rpg->window, 60);
    if (rpg->window == NULL) {
        destroy_rpg(rpg);
        return NULL;
    }
    return rpg;
}

cats_t *create_cats(void)
{
    cats_t *cats = malloc_empty(sizeof(cats_t));

    if (cats == NULL)
        return NULL;
    cats->textures =
        malloc_empty(sizeof(sfTexture *) * (CAT_NAMES_SIZE + 1));
    cats->sprites =
        malloc_empty(sizeof(sfSprite *) * (CAT_NAMES_SIZE + 1));
    cats->hitbox =
        malloc_empty(sizeof(sfCircleShape *) * (CAT_NAMES_SIZE + 1));
    if (cats->sprites == NULL ||
        cats->textures == NULL) {
        destroy_cats(cats);
        return NULL;
    }
    return cats;
}
