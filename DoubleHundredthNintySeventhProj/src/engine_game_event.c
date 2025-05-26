/*
** EPITECH PROJECT, 2021
** game_event
** File description:
** Events basic game calls
*/

#include "engine_rpg.h"
#include "engine_entity.h"
#include "engine_sound.h"
#include "engine_menus.h"
#include "engine_button.h"
#include "engine_sfml_extra.h"

void button_event(game_t *game)
{
    button_t *button;

    for (int i = 0; i != game->buttons.size; i++) {
        button = (button_t *) my_vector_get(&game->buttons, i);
        if (button->state == PRESSED_BUTTON_STATE)
            return button_action(game, button->type);
    }
}

void game_click_left(game_t *game, const sfVector2f mouse_pos)
{
    if (game->state == PLAYING_STATE) {
        if (sfsprite_isvectorinside(game->player.camera_hud.sprite,
            mouse_pos)) {
            player_camera_hud_click(game);
        } else {
            camera_transition_to(&game->camera, mouse_pos, TIPS_SOUND, 2000);
            heal_create(&game->current_map.world_particule, mouse_pos);
        }
    }
}

void game_click(game_t *game, sfMouseButton button)
{
    sfVector2f mouse_pos = sfvector2f_fixmousepositionview(
        game->window,
        game->camera.view,
        (sfVector2f) {
            game->event.mouseButton.x,
            game->event.mouseButton.y
        }
    );

    if (button == sfMouseLeft)
        game_click_left(game, mouse_pos);
}

void game_key(game_t *game)
{
    if (game->event.key.code == sfKeyEscape) {
        if (game->state == INVENTORY_STATE)
            inv_game(game);
        else
            pause_game(game);
    }
    if (game->event.key.code == sfKeyI)
        inv_game(game);
}

void game_event(game_t *game)
{
    while (sfRenderWindow_pollEvent(game->window, &game->event)) {
        if (game->event.type == sfEvtClosed)
            sfRenderWindow_close(game->window);
        if (game->event.type == sfEvtMouseButtonPressed)
            game_click(game, game->event.mouseButton.button);
        if (game->event.type == sfEvtKeyPressed)
            game_key(game);
        if (game->event.type == sfEvtMouseButtonReleased)
            button_event(game);
    }
}