/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

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

  This is the source needed to decode a Creative Labs VOC file into a
  waveform. It's pretty straightforward once you get going. The only
  externally-callable function is Mix_LoadVOC_RW(), which is meant to
  act as identically to SDL_LoadWAV_RW() as possible.

  This file by Ryan C. Gordon (icculus@icculus.org).

  Heavily borrowed from sox v12.17.1's voc.c.
        (http://www.freshmeat.net/projects/sox/)
*/

/* $Id$ */

/* Don't call this directly; use Mix_LoadWAV_RW() for now. */
extern DECLSPEC SDL_AudioSpec *Mix_LoadVOC_RW (SDL_RWops *src, int freesrc,
        SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
