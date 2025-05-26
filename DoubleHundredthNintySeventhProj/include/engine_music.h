/*
** EPITECH PROJECT, 2021
** music
** File description:
** Header of Music for RPG
*/

#ifndef MUSIC_H_
#define MUSIC_H_

#include "engine_vector.h"
#include <stdbool.h>
#include <SFML_Audio.h>

typedef enum music_type
{
    CHILL_MUSIC, PAUSE_MUSIC, CHILL_NOT_SAFE_MUSIC,
    TIME_WASTING_MUSIC, ENEMY_THEME_MUSIC
} music_type;

typedef struct music_s
{
    music_type type;
    sfMusic *music;
} music_t;

/*    MUSIC    */
void music_init(vector_t *musics);
void music_play(vector_t *musics, music_type type, sfBool loop, float volume);
void music_pause(vector_t *musics, music_type type);
void music_destroy(vector_t *musics);

/*    MUSIC_MORE    */
void music_stop_all(vector_t *musics);
void music_stop(vector_t *musics, music_type type);
void music_resume_all(vector_t *musics);
void music_pause_all(vector_t *musics);

#endif /* !MUSIC_H_ */
