/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** defines
*/

#ifndef DEFINES_H_
    #define DEFINES_H_

    #ifndef CAT_NAMES_SIZE
        #define CAT_NAMES_SIZE 8
    #endif
    #ifndef MAP_NAMES_SIZE
        #define MAP_NAMES_SIZE 24
    #endif
    #ifndef MAP_SCALE
        #define MAP_SCALE 2.25
    #endif
    #ifndef CHAR_SPEED
        #define CHAR_SPEED
        #define CHAR_SPEED_SLOW 160
        #define CHAR_SPEED_DEFAULT 300
        #define CHAR_SPEED_FAST 600
    #endif
    #ifndef QUEST_NB
        #define QUEST_NB 2
    #endif

    #ifndef XP_TEST_
        #define XP_TEST_(stats) (stats->xp >= stats->xp_to_next_level)
    #endif

    #ifndef OVER
        #define OVER(a, b) ((int) (a - b) < 0 ? 0 : (a - b))
    #endif

    #ifndef ENNEMY_DISTANCE
        #define ENNEMY_DISTANCE
        #define MOVE_ENNEMY_DISTANCE 200
        #define ATTACK_PLAYER_DISTANCE 150
        #define ATTACK_ENNEMY_DISTANCE 100
    #endif

#endif
