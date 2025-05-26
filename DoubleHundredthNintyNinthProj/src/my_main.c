/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** main
*/

#include <stdlib.h>
#include <unistd.h>
#include <SFML_Audio.h>
#include <stdio.h>
#include <SFML_Graphics.h>
#include <math.h>
#include "my_utils.h"
#include "my_variables.h"
#include "my.h"
#include "my_goto.h"
#include "my_create.h"
#include "my_destroy.h"
#include "my_menu.h"
#include "my_defines.h"

int help(void)
{
    write(1, HELP_CONST, my_strlen(HELP_CONST));
    return 0;
}

void death(rpg_t *rpg)
{
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++)
        rpg->character->cats->is_alive[i] = false;
    print_text(rpg, DEATH_START, DEATH_END, BENJ);
    destroy_perso(rpg->character->perso);
    rpg->character->perso = create_perso(true, rpg->env);
    sfSprite_setPosition(rpg->character->sprite, (sfVector2f)
        {rpg->character->perso->pos_x, rpg->character->perso->pos_y});
    sfSprite_setPosition(rpg->menu->map->sprite
        [rpg->character->perso->id_zone], (sfVector2f)
        {rpg->character->perso->map_pos_x, rpg->character->perso->map_pos_y});
    sfSprite_setPosition(rpg->menu->map->sprite
        [rpg->character->perso->id_zone + 1], (sfVector2f)
        {rpg->character->perso->map_pos_x, rpg->character->perso->map_pos_y});
    if (rpg->character->perso == NULL)
        sfRenderWindow_close(rpg->window);
}

int error(int ac, char **av, char **env)
{
    if (env == NULL || env[0] == NULL ||
        ac > 2 || (ac == 2 && my_strncmp(av[1], "-h", 3)))
        return 84;
    if (ac == 2)
        return help();
    return 0;
}

int main(int ac, char **av, char **env)
{
    rpg_t *rpg = NULL;

    if (error(ac, av, env) == 84)
        return 84;
    rpg = create_rpg();
    rpg->env = env;
    setup_sprites(rpg);
    create_ennemi_health(rpg);
    if (menu(rpg)) {
        goto_house1(rpg);
        print_text(rpg, MAIN_START, MAIN_END, BENJ);
    }
    setup_sounds(rpg);
    setup_claw(rpg);
    main_music(rpg);
    while (sfRenderWindow_isOpen(rpg->window))
        loop(rpg, env);
    destroy_rpg(rpg);
    return 0;
}
