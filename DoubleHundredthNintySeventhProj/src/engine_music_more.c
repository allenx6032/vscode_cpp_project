/*
** EPITECH PROJECT, 2021
** music_more
** File description:
** Music More for RPG
*/

#include "engine_music.h"

void music_resume_all(vector_t *musics)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (sfMusic_getStatus(music->music) == sfPaused)
            sfMusic_play(music->music);
    }
}

void music_pause_all(vector_t *musics)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (sfMusic_getStatus(music->music) == sfPlaying)
            sfMusic_pause(music->music);
    }
}

void music_stop_all(vector_t *musics)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (sfMusic_getStatus(music->music) != sfStopped)
            sfMusic_stop(music->music);
    }
}

void music_stop(vector_t *musics, music_type type)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (music->type == type) {
            if (sfMusic_getStatus(music->music) == sfPlaying)
                sfMusic_stop(music->music);
        }
    }
}