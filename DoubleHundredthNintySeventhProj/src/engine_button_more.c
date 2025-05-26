/*
** EPITECH PROJECT, 2021
** button_more
** File description:
** More Things for Button of RPG
*/

#include <stdlib.h>
#include "engine_rpg.h"
#include "engine_button.h"
#include "engine_teklibc.h"
#include "engine_sfml_extra.h"

void button_update_state(game_t *game, button_t *button)
{
    sfVector2i tmp_mouse_pos =
        sfMouse_getPosition((sfWindow *)game->window);
    sfVector2f mouse_pos =
        sfvector2f_fixmousepositionview(game->window, game->camera.view,
        (sfVector2f) { sfMouse_getPositionRenderWindow(game->window).x,
            sfMouse_getPositionRenderWindow(game->window).y });
    const sfFloatRect frec = sfSprite_getGlobalBounds(button->sprite);

    if (sfFloatRect_contains(&frec,
        mouse_pos.x, mouse_pos.y)){
        if (button->state == IDLE_BUTTON_STATE)
            sound_play(&game->sounds, BUTTON_HOVER_SOUND, sfFalse,
                (float) my_atoi(config_get(game->config, "volume")));
        button->state = HOVER_BUTTON_STATE;
        if (sfMouse_isButtonPressed(game->event.mouseButton.button)) {
            button->state = PRESSED_BUTTON_STATE;
        }
    } else {
        button->state = IDLE_BUTTON_STATE;
    }
}

char *button_str_from_type_settings(config_t *config, button_type type)
{
    switch (type)
    {
        case VOLUME_SETTINGS_BUTTON:
            return my_strcat("Volume : ",
                config_get(config, "volume"));
        case FULLSCREEN_SETTINGS_BUTTON:
            return my_strcmp(config_get(config,
                "fullscreen"), "1")  ? "Fullscreen : No" : "Fullscreen : Yes";
        case FPS_SETTINGS_BUTTON:
            return my_strcat("FPS : ",
                config_get(config, "fps"));
        default:
            return "BUTTON ERROR";
    }
}

char *button_str_from_type(config_t *config, button_type type)
{
    switch (type)
    {
        case START_BUTTON:
            return "Start !";
        case SETTINGS_BUTTON:
            return "Settings";
        case HOW_TO_PLAY_BUTTON:
            return "How to Play";
        case PAUSE_BUTTON:
            return "Resume Game";
        case EXIT_BUTTON:
            return "Exit Game";
        case MAIN_MENU_BUTTON:
            return "Main Menu";
        case VOLUME_SETTINGS_BUTTON: case FULLSCREEN_SETTINGS_BUTTON:
            case FPS_SETTINGS_BUTTON:
            return button_str_from_type_settings(config, type);
        default:
            return "BUTTON ERROR";
    }
    return NULL;
}