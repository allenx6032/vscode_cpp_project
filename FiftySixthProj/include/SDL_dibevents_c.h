/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_lowvideo.h"

/* Variables and functions exported by SDL_dibevents.c to other parts 
   of the native video subsystem (SDL_dibvideo.c)
*/
extern LONG
 DIB_HandleMessage(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern int DIB_CreateWindow(_THIS);
extern void DIB_DestroyWindow(_THIS);

extern void DIB_PumpEvents(_THIS);
extern void DIB_InitOSKeymap(_THIS);
