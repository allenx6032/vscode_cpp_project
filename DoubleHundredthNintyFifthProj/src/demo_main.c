/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#include <SFML_Graphics.h>
#include <SFML_Audio.h>
#include "engine_event_handling.h"
#include "engine_sprite_texture_utils.h"
#include "engine_window_utils.h"
#include "engine_app.h"
#include "engine_player.h"
#include "engine_fight_view.h"
#include "engine_decor.h"
#include <stdio.h>


sfBool check_win(game_app *app)
{
    if (app->player->life <= 0) {
        printf("Player 2 win\n");
        return sfTrue;
    }
    if (app->player2->life <= 0) {
        printf("Player 1 win\n");
        return sfTrue;
    }
    return sfFalse;
}

typedef enum {
    MENU,
    FIGHT
} GameState;

int main(void)
{
    sfRenderWindow *window = init_window();
    sfEvent event;
    game_app *app = init_game_app();
    sfClock* clock = sfClock_create();
    float delta_time;
    sfSprite **life_sprites_player2 = get_player_life_sprites(app->player2, sfTrue);
    sfSprite **life_sprites_player = get_player_life_sprites(app->player, sfFalse);
    sfMusic* music;
    music = sfMusic_createFromFile("assets/coloss.mp3");
    sfMusic_play(music);
    GameState game_state = FIGHT;

    sfRenderWindow_setFramerateLimit(window, 120);
    while (sfRenderWindow_isOpen(window)) {
        while (sfRenderWindow_pollEvent(window, &event)) {
            check_event(window, &event);
        }
        switch (game_state)
        {
            case MENU:
                break;
            
            default:
                fight_view__render(window, app, clock, event, life_sprites_player, life_sprites_player2);
                break;
        }
        if(check_win(app))
            break;
        sfRenderWindow_display(window);
    }
    sfSprite_destroy(app->player->player_entity->sprite);
    sfSprite_destroy(app->player2->player_entity->sprite);
    sfSprite_destroy(app->decor);
    sfMusic_destroy(music);
    sfRenderWindow_destroy(window);
    return 0;
}
