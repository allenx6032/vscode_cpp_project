/*
** EPITECH PROJECT, 2021
** button
** File description:
** Button for RPG
*/

#ifndef BUTTON_H_
#define BUTTON_H_

#include <SFML_Graphics.h>
#include <SFML_Audio.h>
#include <SFML_System.h>
#include <SFML_Clock.h>

typedef enum button_state
{
    IDLE_BUTTON_STATE, HOVER_BUTTON_STATE, PRESSED_BUTTON_STATE
} button_state;

typedef enum button_type
{
    PAUSE_BUTTON, EXIT_BUTTON, MAIN_MENU_BUTTON,
    START_BUTTON, SETTINGS_BUTTON, HOW_TO_PLAY_BUTTON,
    VOLUME_SETTINGS_BUTTON, FULLSCREEN_SETTINGS_BUTTON,
    FPS_SETTINGS_BUTTON
} button_type;

typedef struct button_s
{
    button_state state;
    button_type type;
    sfVector2f position;
    sfSprite *sprite;
    sfTexture *texture;
    sfText *text;
} button_t;

void button_init(game_t *game);
void button_create(game_t *game, button_type type, sfVector2f position);
void button_update(game_t *game);
void button_render(game_t *game);
void button_destroy(game_t *game);

void button_update_state(game_t *game, button_t *button);
char *button_str_from_type(config_t *config, button_type type);

void button_action(game_t *game, button_type type);

#endif /* !BUTTON_H_ */
