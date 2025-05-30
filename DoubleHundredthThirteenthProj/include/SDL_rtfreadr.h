/*
  SDL_rtf:  A companion library to SDL for displaying RTF format text
  Copyright (C) 2003-2024 Sam Lantinga <slouken@libsdl.org>

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

/* $Id$ */

#ifndef _SDL_RTFREADR_H
#define _SDL_RTFREADR_H

#include "SDL_rtftype.h"
#include "SDL.h"

int ecReflowText(RTF_Context *ctx, int width);
int ecRenderText(RTF_Context *ctx, const SDL_Rect *rect, int yOffset);

#endif /* _SDL_RTFREADR_H */
