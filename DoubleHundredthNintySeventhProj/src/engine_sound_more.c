/*
** EPITECH PROJECT, 2021
** sound
** File description:
** Sound More for RPG
*/

#include "engine_sound.h"

void sound_pause(sound_t *sound)
{
    if (sfSound_getStatus(sound->sound) == sfPlaying)
        sfSound_pause(sound->sound);
}

void sound_resume(sound_t *sound)
{
    if (sfSound_getStatus(sound->sound) == sfPaused)
        sfSound_play(sound->sound);
}

void sound_pause_all(vector_t *sounds)
{
    sound_t *sound;

    for (int i = 0; i != sounds->size; i++) {
        sound = my_vector_get(sounds, i);
        if (sfSound_getStatus(sound->sound) == sfPlaying)
            sfSound_pause(sound->sound);
    }
}

void sound_stop_all(vector_t *sounds)
{
    sound_t *sound;

    for (int i = 0; i != sounds->size; i++) {
        sound = my_vector_get(sounds, i);
        if (sfSound_getStatus(sound->sound) != sfStopped)
            sfSound_stop(sound->sound);
    }
}

void sound_resume_all(vector_t *sounds)
{
    sound_t *sound;

    for (int i = 0; i != sounds->size; i++) {
        sound = my_vector_get(sounds, i);
        if (sfSound_getStatus(sound->sound) == sfPaused)
            sfSound_play(sound->sound);
    }
}