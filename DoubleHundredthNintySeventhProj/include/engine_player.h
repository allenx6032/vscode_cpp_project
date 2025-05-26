/*
** EPITECH PROJECT, 2021
** player
** File description:
** Header of Player for RPG
*/

#ifndef PLAYER_H_
#define PLAYER_H_

#include "engine_map.h"
#include "engine_inventory.h"
#include <SFML_Graphics.h>

typedef struct player_camera_hud_s
{
    sfSprite *sprite;
    sfSprite *sprite_back;
    sfSprite *sprite_back_rotate;
    sfTexture *texture;
    sfTexture *texture_back;
    sfTexture *texture_back_rotate;
    sfVector2f position;
    sfClock *anim_clock;
    sfClock *anim_clock_rotate;
    bool zoom;
} player_camera_hud_t;

typedef struct player_life_hud_s
{
    sfSprite *icon_sprite;
    sfSprite *life_box_sprite;
    sfSprite *life_hud_sprite;
    sfTexture *texture;
} player_life_hud_t;

typedef struct player_s
{
    sfSprite *sprite;
    sfTexture *texture;
    sfVector2f position;
    sfVector2f prev_position;
    sfClock *anim_clock;
    sfClock *move_clock;
    int life;
    player_life_hud_t life_hud;
    player_camera_hud_t camera_hud;
    inventory_items_t items;
} player_t;

/*    PLAYER    */
void player_init(player_t *player);
void player_update(map_data_t *current_map, player_t *player);
void player_render(sfRenderWindow *window, player_t player);
void player_destroy(player_t *player);

/*    PLAYER_MORE    */
void player_move(map_data_t *current_map, player_t *player);
void player_animate(player_t *player, world_entity_t *world_entity);
void player_animate_idle(player_t *player);


/*    LIFE_HUD    */
void player_life_hud_init(player_life_hud_t *life_hud);
void player_life_hud_update(player_t *player, sfView *camera);
void player_life_hud_render(sfRenderWindow *window,
    player_life_hud_t life_hud);
void player_life_hud_destroy(player_life_hud_t *life_hud);

/*   CAMERA HUD    */
void player_camera_hud_init(player_camera_hud_t *camera_hud);
void player_camera_hud_update(player_camera_hud_t *camera_hud,
    game_t *game);
void player_camera_hud_render(sfRenderWindow *window,
    player_camera_hud_t camera_hud);
void player_camera_hud_destroy(player_camera_hud_t *camera_hud);

/*   CAMERA HUD MORE   */
void player_camera_hud_animate(player_camera_hud_t *camera_hud,
    game_t *game);
void player_camera_hud_click(game_t *game);
void player_camera_hud_update_asset(player_camera_hud_t *camera_hud,
    game_t *game);
void player_camera_hud_init_asset(player_camera_hud_t *camera_hud);

#endif /* !PLAYER_H_ */