/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** loadings
*/

#include "my_loadings.h"

void small_loading(sfRenderWindow *window)
{
    sfClock *clock = sfClock_create();

    print_progression(window, 0);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 1000) {
        print_progression(window, sfTime_asMilliseconds(
            sfClock_getElapsedTime(clock)) / 1000.0f * 100);
    }
    print_progression(window, 100);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 1500);
    sfClock_destroy(clock);
}

void medium_loading(sfRenderWindow *window)
{
    sfClock *clock = sfClock_create();

    print_progression(window, 0);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 3000) {
        print_progression(window, sfTime_asMilliseconds(
            sfClock_getElapsedTime(clock)) / 3000.0f * 100);
    }
    print_progression(window, 100);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 4500);
    sfClock_destroy(clock);
}

void long_loading(sfRenderWindow *window)
{
    sfClock *clock = sfClock_create();

    print_progression(window, 0);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 6000) {
        print_progression(window, sfTime_asMilliseconds(
            sfClock_getElapsedTime(clock)) / 3000.0f * 100);
    }
    sfClock_restart(clock);
    print_progression(window, 0);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 6000) {
        print_progression(window, (sfTime_asMilliseconds(
            sfClock_getElapsedTime(clock)) / 3000.0f * 100) - 100);
    }
    print_progression(window, 100);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 10000);
    sfClock_destroy(clock);
}

void update_loading(sfRenderWindow *window)
{
    sfClock *clock = sfClock_create();

    print_progression(window, 0);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 500);
    print_progression(window, 30);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 3500);
    print_progression(window, 69);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 4500);
    print_progression(window, 100);
    while (sfTime_asMilliseconds(sfClock_getElapsedTime(clock)) < 6000);
    sfClock_destroy(clock);
}
