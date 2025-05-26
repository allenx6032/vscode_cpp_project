/*
** EPITECH PROJECT, 2023
** GOOD_VER
** File description:
** mouse2.c
*/
#include "my_framebuffer.h"
all_t *part_five(all_t *all)
{
    if (all->count % 2 == 0 && all->count != 1) {
        sfMusic_play(all->music);
        all->count++;
    } else {
        all->count += 1;
        sfMusic_pause(all->music);
    }
    return all;
}

all_t *part_four(sfMouseButtonEvent event, all_t *all, menu_t *menu)
{
    sfFloatRect Boundqris = sfSprite_getGlobalBounds(all->scene.music.sprite);
    sfFloatRect Boundsis = sfSprite_getGlobalBounds(all->scene.quit.sprite);

    if (sfFloatRect_contains(&Boundqris, event.x, event.y)) {
        all = part_five(all);
    }
    if (sfFloatRect_contains(&Boundsis, event.x, event.y)) {
        destroy_everything(*all, *menu);
    }
    return all;
}

all_t *part_three(sfMouseButtonEvent event, all_t *all, menu_t *menu)
{
    sfFloatRect Boundcins = sfSprite_getGlobalBounds(all->scene.start.sprite);

    if (sfFloatRect_contains(&Boundcins, event.x, event.y)) {
        menu->isoff = 2;
        all->scene.duck.position.x = -150;
        sfMusic_destroy(all->music);
        all->music = sfMusic_createFromFile("Music/Knights of Favonius.ogg");
        sfMusic_play(all->music);
        choose_screen(menu, all);
    }
    all = part_four(event, all, menu);
    return all;
}

all_t *part_two(sfMouseButtonEvent event, all_t *all, menu_t *menu)
{
    sfFloatRect Boundtris = sfSprite_getGlobalBounds(all->scene.pause.sprite);

    if (sfFloatRect_contains(&Boundtris, event.x, event.y)) {
        if (menu->isoff == 1 || (menu->isoff % 2 == 0 && menu->isoff != 0))
            menu->isoff = 5;
        else
            menu->isoff = 2;
        choose_screen(menu, all);
    }
    all = part_three(event, all, menu);
    return all;
}
