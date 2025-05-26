/*
** EPITECH PROJECT, 2021
** window
** File description:
** Game Window
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"

void window_init(sfRenderWindow **window, config_t *config)
{
    int fullscreen = my_atoi(
            config_get(config, "fullscreen"));

    if (!fullscreen) {
        *window = sfRenderWindow_create(VIDEO_MODE, "MyRPG",
            sfClose | sfResize, NULL);
    } else {
        *window = sfRenderWindow_create(
            sfVideoMode_getDesktopMode(), "MyRPG",
            sfClose | sfResize | sfFullscreen, NULL
        );
    }
    sfRenderWindow_setFramerateLimit(*window,
        my_atoi(config_get(config, "fps")));
    sfRenderWindow_setMouseCursorVisible(*window, sfTrue);
}

void window_destroy(sfRenderWindow **window)
{
    sfRenderWindow_destroy(*window);
}