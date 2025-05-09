/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

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
#include "SDL_windows.h"

/* Return true if the RawInput driver is enabled */
extern SDL_bool RAWINPUT_IsEnabled();

/* Return true if a RawInput device is present and supported as a joystick */
extern SDL_bool RAWINPUT_IsDevicePresent(Uint16 vendor_id, Uint16 product_id, Uint16 version, const char *name);

/* Registers for input events */
extern SDL_bool RAWINPUT_RegisterNotifications(HWND hWnd);
extern void RAWINPUT_UnregisterNotifications();

/* Returns 0 if message was handled */
extern LRESULT CALLBACK RAWINPUT_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/* vi: set ts=4 sw=4 expandtab: */
