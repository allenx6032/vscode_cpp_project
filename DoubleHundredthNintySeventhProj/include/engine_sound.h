/*
** EPITECH PROJECT, 2021
** sound
** File description:
** Sound Header for RPG
*/

#ifndef SOUND_H_
#define SOUND_H_

#include <stdbool.h>
#include <SFML_Audio.h>
#include "engine_vector.h"

typedef enum sound_type
{
    TIPS_SOUND, CAMERA_OUT_SOUND, CAMERA_IN_SOUND, BUTTON_HOVER_SOUND,
    BUTTON_ACTION_SOUND, NEW_ITEM_SOUND
} sound_type;

typedef struct sound_s
{
    sound_type type;
    sfSoundBuffer *buffer;
    sfSound *sound;
} sound_t;

void sound_init(vector_t *sounds);
void sound_play(vector_t *sounds, sound_type type, sfBool loop, float volume);
void sound_destroy(vector_t *sounds);

/*    SOUND MORE    */
void sound_pause(sound_t *sound);
void sound_resume(sound_t *sound);
void sound_stop_all(vector_t *sounds);
void sound_pause_all(vector_t *sounds);
void sound_resume_all(vector_t *sounds);

#endif /* !SOUND_H_ */
