/*
** EPITECH PROJECT, 2020
** main
** File description:
** Main Function of MyRPG
*/

#include <stdlib.h>
#include <time.h>
#include "engine_teklibc.h"
#include "engine_rpg.h"
#include "engine_config.h"

int main(int ac, char **av, char **envp)
{
    time_t t;

    if (envp[0] == NULL)
        return 84;
    game_t *game = malloc(sizeof(game_t));
    if (game == NULL)
        return 84;
    srand((unsigned) time(&t));
    game->config = config_load("./rpg.ini");
    load_rpg_config(game->config);
    game_init(game);
    while (sfRenderWindow_isOpen(game->window)) {
        game_update(game);
        game_render(game);
    }
    config_save(game->config, "./rpg.ini");
    game_destroy(game);
    free(game);
    return 0;
}