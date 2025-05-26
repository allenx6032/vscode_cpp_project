/*
** EPITECH PROJECT, 2021
** player_more
** File description:
** Player related manipulation moved for coding style
*/

#include "engine_map.h"
#include "engine_rpg.h"
#include "engine_player.h"

void player_move_math_more(map_data_t *current_map, player_t *player,
    sfVector2f *pos, const float move)
{
    if (sfKeyboard_isKeyPressed(sfKeyD))
        pos->x += move;
    if (!map_sprite_can_move_at(current_map, player->sprite, *pos))
        pos->x -= move;
    if (sfKeyboard_isKeyPressed(sfKeyZ))
        pos->y -= move;
    if (!map_sprite_can_move_at(current_map, player->sprite, *pos))
        pos->y += move;
}

sfVector2f player_move_math(map_data_t *current_map, player_t *player)
{
    sfVector2f pos = sfSprite_getPosition(player->sprite);
    const float elapsed_time_sec =
        sfTime_asSeconds(sfClock_getElapsedTime(player->move_clock));
    const float move = 75.0f * elapsed_time_sec;

    if (sfKeyboard_isKeyPressed(sfKeyS))
        pos.y += move;
    if (!map_sprite_can_move_at(current_map, player->sprite, pos))
        pos.y -= move;
    if (sfKeyboard_isKeyPressed(sfKeyQ))
        pos.x -= move;
    if (!map_sprite_can_move_at(current_map, player->sprite, pos))
        pos.x += move;
    player_move_math_more(current_map, player, &pos, move);
    return pos;
}

void player_move(map_data_t *current_map, player_t *player)
{
    const sfVector2f move_pos = player_move_math(current_map, player);
    sfIntRect rec = sfSprite_getTextureRect(player->sprite);

    if (!map_sprite_can_move_at(current_map, player->sprite, move_pos))
        return;
    player->prev_position = player->position;
    player->position = move_pos;
    if (sfKeyboard_isKeyPressed(sfKeyS))
        rec.top = 0;
    if (sfKeyboard_isKeyPressed(sfKeyQ))
        rec.top = 32;
    if (sfKeyboard_isKeyPressed(sfKeyD))
        rec.top = 64;
    if (sfKeyboard_isKeyPressed(sfKeyZ))
        rec.top = 96;
    sfSprite_setTextureRect(player->sprite, rec);
}

void player_animate(player_t *player, world_entity_t *world_entity)
{
    sfIntRect rec = sfSprite_getTextureRect(player->sprite);

    if (!sfKeyboard_isKeyPressed(sfKeySpace)) {
        if (rec.left >= 160)
            rec.left = 0;
        else
            rec.left += 32;
    } else {
        if (rec.left >= 320)
            rec.left = 192;
        else
            rec.left += 32;
        damage_entity_arrond(world_entity,
            player->position, 50, 50);
    }
    sfSprite_setTextureRect(player->sprite, rec);
}

void player_animate_idle(player_t *player)
{
    sfIntRect rec = sfSprite_getTextureRect(player->sprite);

    rec.left = 32;
    sfSprite_setTextureRect(player->sprite, rec);
}