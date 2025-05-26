/*
** EPITECH PROJECT, 2021
** sound
** File description:
** Sound for RPG
*/

#include <stdlib.h>
#include "engine_sound.h"

void sound_create_intern(char *sound_path, sound_type type,
    vector_t *sounds)
{
    sound_t *sound = malloc(sizeof(sound_t));

    if (sound == NULL)
        return;
    sound->sound = sfSound_create();
    sound->buffer = sfSoundBuffer_createFromFile(sound_path);
    sfSound_setBuffer(sound->sound, sound->buffer);
    sound->type = type;
    my_vector_add(sounds, sound);
}

void sound_init(vector_t *sounds)
{
    my_vector_init(sounds);
    sound_create_intern("./data/sound/tips.ogg", TIPS_SOUND, sounds);
    sound_create_intern("./data/sound/camera_out.ogg",
        CAMERA_OUT_SOUND, sounds);
    sound_create_intern("./data/sound/camera_in.ogg",
        CAMERA_IN_SOUND, sounds);
    sound_create_intern("./data/sound/button_hover.ogg",
        BUTTON_HOVER_SOUND, sounds);
    sound_create_intern("./data/sound/button_action.ogg",
        BUTTON_ACTION_SOUND, sounds);
    sound_create_intern("./data/sound/new_item.ogg",
        NEW_ITEM_SOUND, sounds);
}

void sound_play(vector_t *sounds, sound_type type, sfBool loop, float volume)
{
    sound_t *sound;

    for (int i = 0; i != sounds->size; i++) {
        sound = my_vector_get(sounds, i);
        if (sound->type == type) {
            if (sfSound_getStatus(sound->sound) == sfPlaying)
                sfSound_stop(sound->sound);
            sfSound_play(sound->sound);
            sfSound_setLoop(sound->sound, loop);
            sfSound_setVolume(sound->sound, volume);
        }
    }
}

void sound_destroy(vector_t *sounds)
{
    sound_t *sound;

    for (int i = 0; i != sounds->size; i++) {
        sound = my_vector_get(sounds, i);
        sfSoundBuffer_destroy(sound->buffer);
        sfSound_destroy(sound->sound);
        free(sound);
    }
    my_vector_free(sounds);
}