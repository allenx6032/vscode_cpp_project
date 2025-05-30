/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

/* Functions and variables exported from SDL_audio.c for SDL_sysaudio.c */

/* Functions to get a list of "close" audio formats */
extern SDL_AudioFormat SDL_FirstAudioFormat(SDL_AudioFormat format);
extern SDL_AudioFormat SDL_NextAudioFormat(void);

/* Function to calculate the size and silence for a SDL_AudioSpec */
extern void SDL_CalculateAudioSpec(SDL_AudioSpec * spec);

/* this is used internally to access some autogenerated code. */
typedef struct
{
    SDL_AudioFormat src_fmt;
    SDL_AudioFormat dst_fmt;
    SDL_AudioFilter filter;
} SDL_AudioTypeFilters;
extern const SDL_AudioTypeFilters sdl_audio_type_filters[];

/* this is used internally to access some autogenerated code. */
typedef struct
{
    SDL_AudioFormat fmt;
    int channels;
    int upsample;
    int multiple;
    SDL_AudioFilter filter;
} SDL_AudioRateFilters;
extern const SDL_AudioRateFilters sdl_audio_rate_filters[];

/* vi: set ts=4 sw=4 expandtab: */
