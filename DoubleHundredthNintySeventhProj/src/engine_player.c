/*
** EPITECH PROJECT, 2021
** player
** File description:
** Player basic game calls
*/

/*
    ProTips sa maman
    sfIntRec dans l'ordre: left, top, width, height
*/

#include "engine_player.h"
#include "engine_sfml_extra.h"

void inventory_init(inventory_items_t *items)
{
    items->map = true;
    items->map_texture =
        sfTexture_createFromFile("./data/building_more.png", NULL);
    items->map_rect = (sfIntRect) { 483, 405, 90, 54 };
    items->potion = true;
    items->potion_texture =
        sfTexture_createFromFile("./data/building_more.png", NULL);
    items->potion_rect = (sfIntRect) { 1116, 350, 30, 30 };
    items->key = false;
    items->key_texture =
        sfTexture_createFromFile("./data/building_more.png", NULL);
    items->key_rect = (sfIntRect) { 1210, 6, 24, 24 };
}

void player_init(player_t *player)
{
    inventory_init(&player->items);
    player->sprite = sfSprite_create();
    player->position = (sfVector2f){ 600, 600 };
    player->prev_position = (sfVector2f){ 600, 600 };
    player->texture = sfTexture_createFromFile("./data/player.png", NULL);
    sfSprite_setTexture(player->sprite, player->texture, sfFalse);
    sfSprite_setPosition(player->sprite, player->position);
    sfSprite_setTextureRect(player->sprite, (sfIntRect){0, 0, 32, 32});
    player->anim_clock = sfClock_create();
    player->move_clock = sfClock_create();
    player->life = 100;
}

void player_update(map_data_t *current_map, player_t *player)
{
    if (sfTime_asSeconds(
        sfClock_getElapsedTime(player->anim_clock)) > 0.25f){
        if (sfvector2f_isdifferent(player->position, player->prev_position)){
            player_animate(player, &current_map->world_entity);
        } else {
            player_animate_idle(player);
        }
        sfClock_restart(player->anim_clock);
    }
    player_move(current_map, player);
    sfClock_restart(player->move_clock);
    sfSprite_setPosition(player->sprite, player->position);
}

void player_render(sfRenderWindow *window, player_t player)
{
    sfRenderWindow_drawSprite(window, player.sprite, NULL);
}

void player_destroy(player_t *player)
{
    sfClock_destroy(player->anim_clock);
    sfClock_destroy(player->move_clock);
    sfSprite_destroy(player->sprite);
    sfTexture_destroy(player->texture);
    sfTexture_destroy(player->items.map_texture);
    sfTexture_destroy(player->items.potion_texture);
    sfTexture_destroy(player->items.key_texture);
}