/*
** EPITECH PROJECT, 2021
** music
** File description:
** An easy and optimized way to play music
*/

#include "engine_music.h"
#include <stdlib.h>

void music_create_intern(char *music_path, music_type type,
    vector_t *musics)
{
    music_t *music = malloc(sizeof(music_t));

    if (music == NULL)
        return;
    music->music = sfMusic_createFromFile(music_path);
    music->type = type;
    my_vector_add(musics, music);
}

void music_init(vector_t *musics)
{
    my_vector_init(musics);
    music_create_intern("./data/music/chill.ogg", CHILL_MUSIC, musics);
    music_create_intern("./data/music/pause.ogg", PAUSE_MUSIC, musics);
    music_create_intern("./data/music/chill_but_not_safe.ogg",
        CHILL_NOT_SAFE_MUSIC, musics);
    music_create_intern("./data/music/time_wasting.ogg",
        TIME_WASTING_MUSIC, musics);
    music_create_intern("./data/music/enemy_theme_music.ogg",
        ENEMY_THEME_MUSIC, musics);
}

void music_play(vector_t *musics, music_type type, sfBool loop, float volume)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (music->type == type) {
            if (sfMusic_getStatus(music->music) == sfPlaying)
                sfMusic_stop(music->music);
            sfMusic_play(music->music);
            sfMusic_setLoop(music->music, loop);
            sfMusic_setVolume(music->music, volume);
        }
    }
}

void music_pause(vector_t *musics, music_type type)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        if (music->type == type)
            if (sfMusic_getStatus(music->music) == sfPlaying)
                sfMusic_pause(music->music);
    }
}

void music_destroy(vector_t *musics)
{
    music_t *music;

    for (int i = 0; i != musics->size; i++) {
        music = my_vector_get(musics, i);
        sfMusic_destroy(music->music);
        free(music);
    }
    my_vector_free(musics);
}