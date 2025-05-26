/*
** EPITECH PROJECT, 2023
** menu
** File description:
** menu
*/

#include <SFML_Graphics.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "my_menu.h"
#include "my_typedef.h"
#include "my_utils.h"
#include "my_create.h"

void video_init(game_menu_t *game, rpg_t *rpg)
{
    game->video->window = rpg->window;
    game->audio->meow_buffer = sfSoundBuffer_createFromFile(
        "assets/menu/meow_click.ogg");
    game->audio->meow_click = sfSound_create();
    sfSound_setBuffer(game->audio->meow_click, game->audio->meow_buffer);
    game->audio->click_buffer = sfSoundBuffer_createFromFile(
        "assets/menu/menu_click.ogg");
    game->audio->menu_click = sfSound_create();
    sfSound_setBuffer(game->audio->menu_click, game->audio->click_buffer);
    game->audio->menu_song = sfMusic_createFromFile(
        "assets/menu/menu_song.ogg");
    sfMusic_play(game->audio->menu_song);
    sfMusic_setLoop(game->audio->menu_song, sfTrue);
}

void menu_sprites_display(game_menu_t *game)
{
        sfRenderWindow_drawSprite(game->video->window,
        game->sprites->backgroundSprite, NULL);
        sfRenderWindow_drawText(game->video->window,
        game->sprites->title_text, NULL);
        sfRenderWindow_drawSprite(game->video->window,
        game->sprites->start_button, NULL);
        sfRenderWindow_drawSprite(game->video->window,
        game->sprites->settings_button, NULL);
        sfRenderWindow_drawSprite(game->video->window,
        game->sprites->exit_button, NULL);
}

bool display_menu(game_menu_t *game, rpg_t *rpg)
{
    create_menu_part_1(game);
    bool test = false;

    while (sfRenderWindow_isOpen(game->video->window) &&
        rpg->character->perso == NULL) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(game->video->window, &event)) {
            test = event_loop(game, &event, rpg) || test;
        }
        sfRenderWindow_clear(game->video->window, sfBlack);
        menu_sprites_display(game);
        sfRenderWindow_display(game->video->window);
    }
    return test;
}

static void destroy_menu(game_menu_t *game)
{
    sfRectangleShape_destroy(game->sprites->yes_button);
    sfRectangleShape_destroy(game->sprites->no_button);
    sfText_destroy(game->sprites->resume_text);
    sfSprite_destroy(game->sprites->start_button);
    sfSprite_destroy(game->sprites->settings_button);
    sfSprite_destroy(game->sprites->exit_button);
    sfSound_destroy(game->audio->menu_click);
    sfMusic_destroy(game->audio->menu_song);
    sfText_destroy(game->sprites->title_text);
    sfFont_destroy(game->sprites->font);
    sfTexture_destroy(game->sprites->backgroundTexture);
    sfSprite_destroy(game->sprites->backgroundSprite);
}

bool menu(rpg_t *rpg)
{
    game_menu_t *game = malloc_empty(sizeof(game_menu_t));
    bool test;

    game->video = malloc_empty(sizeof(menu_video_t));
    game->audio = malloc_empty(sizeof(menu_audio_t));
    game->sprites = malloc_empty(sizeof(menu_sprites_t));
    video_init(game, rpg);
    test = display_menu(game, rpg);
    destroy_menu(game);
    free(game->audio);
    free(game->video);
    free(game->sprites);
    free(game);
    return test;
}
