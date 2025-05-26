/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** loadings
*/

#ifndef LOADINGS_H_
    #define LOADINGS_H_

    #include <SFML_Graphics.h>

void small_loading(sfRenderWindow *window);
void medium_loading(sfRenderWindow *window);
void update_loading(sfRenderWindow *window);
void long_loading(sfRenderWindow *window);
void print_progression(sfRenderWindow *window, int percent);

#endif
