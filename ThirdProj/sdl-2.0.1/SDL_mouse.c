/*
  Simple DirectMedia Layer
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
*/
#include "SDL_config.h"

/* General mouse handling code for SDL */

#include "SDL_assert.h"
#include "SDL_events.h"
#include "SDL_events_c.h"
#include "default_cursor.h"
#include "SDL_sysvideo.h"

/* #define DEBUG_MOUSE */

/* The mouse state */
static SDL_Mouse SDL_mouse;

static int
SDL_PrivateSendMouseMotion(SDL_Window * window, SDL_MouseID mouseID, int relative, int x, int y);

/* Public functions */
int
SDL_MouseInit(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->cursor_shown = SDL_TRUE;

    return (0);
}

void
SDL_SetDefaultCursor(SDL_Cursor * cursor)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->def_cursor = cursor;
    if (!mouse->cur_cursor) {
        SDL_SetCursor(cursor);
    }
}

SDL_Mouse *
SDL_GetMouse(void)
{
    return &SDL_mouse;
}

SDL_Window *
SDL_GetMouseFocus(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    return mouse->focus;
}

void
SDL_ResetMouse(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    Uint8 i;

#ifdef DEBUG_MOUSE
    printf("Resetting mouse\n");
#endif
    for (i = 1; i <= sizeof(mouse->buttonstate)*8; ++i) {
        if (mouse->buttonstate & SDL_BUTTON(i)) {
            SDL_SendMouseButton(mouse->focus, mouse->mouseID, SDL_RELEASED, i);
        }
    }
    SDL_assert(mouse->buttonstate == 0);
}

void
SDL_SetMouseFocus(SDL_Window * window)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (mouse->focus == window) {
        return;
    }

    /* Actually, this ends up being a bad idea, because most operating
       systems have an implicit grab when you press the mouse button down
       so you can drag things out of the window and then get the mouse up
       when it happens.  So, #if 0...
    */
#if 0
    if (mouse->focus && !window) {
        /* We won't get anymore mouse messages, so reset mouse state */
        SDL_ResetMouse();
    }
#endif

    /* See if the current window has lost focus */
    if (mouse->focus) {
        SDL_SendWindowEvent(mouse->focus, SDL_WINDOWEVENT_LEAVE, 0, 0);
    }

    mouse->focus = window;

    if (mouse->focus) {
        SDL_SendWindowEvent(mouse->focus, SDL_WINDOWEVENT_ENTER, 0, 0);
    }

    /* Update cursor visibility */
    SDL_SetCursor(NULL);
}

/* Check to see if we need to synthesize focus events */
static SDL_bool
SDL_UpdateMouseFocus(SDL_Window * window, int x, int y, Uint32 buttonstate)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    int w, h;
    SDL_bool inWindow;

    SDL_GetWindowSize(window, &w, &h);
    if (x < 0 || y < 0 || x >= w || y >= h) {
        inWindow = SDL_FALSE;
    } else {
        inWindow = SDL_TRUE;
    }

/* Linux doesn't give you mouse events outside your window unless you grab
   the pointer.

   Windows doesn't give you mouse events outside your window unless you call
   SetCapture().

   Both of these are slightly scary changes, so for now we'll punt and if the
   mouse leaves the window you'll lose mouse focus and reset button state.
*/
#ifdef SUPPORT_DRAG_OUTSIDE_WINDOW
    if (!inWindow && !buttonstate) {
#else
    if (!inWindow) {
#endif
        if (window == mouse->focus) {
#ifdef DEBUG_MOUSE
            printf("Mouse left window, synthesizing move & focus lost event\n");
#endif
            SDL_PrivateSendMouseMotion(window, mouse->mouseID, 0, x, y);
            SDL_SetMouseFocus(NULL);
        }
        return SDL_FALSE;
    }

    if (window != mouse->focus) {
#ifdef DEBUG_MOUSE
         printf("Mouse entered window, synthesizing focus gain & move event\n");
#endif
         SDL_SetMouseFocus(window);
         SDL_PrivateSendMouseMotion(window, mouse->mouseID, 0, x, y);
    }
    return SDL_TRUE;
}

int
SDL_SendMouseMotion(SDL_Window * window, SDL_MouseID mouseID, int relative, int x, int y)
{
    if (window && !relative) {
        SDL_Mouse *mouse = SDL_GetMouse();
        if (!SDL_UpdateMouseFocus(window, x, y, mouse->buttonstate)) {
            return 0;
        }
    }

    return SDL_PrivateSendMouseMotion(window, mouseID, relative, x, y);
}

static int
SDL_PrivateSendMouseMotion(SDL_Window * window, SDL_MouseID mouseID, int relative, int x, int y)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    int posted;
    int xrel;
    int yrel;
    int x_max = 0, y_max = 0;

    /* relative motion is calculated regarding the system cursor last position */
    if (relative) {
        xrel = x;
        yrel = y;
        x = (mouse->last_x + x);
        y = (mouse->last_y + y);
    } else {
        xrel = x - mouse->last_x;
        yrel = y - mouse->last_y;
    }

    /* Drop events that don't change state */
    if (!xrel && !yrel) {
#ifdef DEBUG_MOUSE
        printf("Mouse event didn't change state - dropped!\n");
#endif
        return 0;
    }

    /* Update internal mouse coordinates */
    if (mouse->relative_mode == SDL_FALSE) {
        mouse->x = x;
        mouse->y = y;
    } else {
        mouse->x += xrel;
        mouse->y += yrel;
    }

    SDL_GetWindowSize(mouse->focus, &x_max, &y_max);
    --x_max;
    --y_max;

    /* make sure that the pointers find themselves inside the windows */
    if (mouse->x > x_max) {
        mouse->x = x_max;
    }
    if (mouse->x < 0) {
        mouse->x = 0;
    }

    if (mouse->y > y_max) {
        mouse->y = y_max;
    }
    if (mouse->y < 0) {
        mouse->y = 0;
    }

    mouse->xdelta += xrel;
    mouse->ydelta += yrel;

    /* Move the mouse cursor, if needed */
    if (mouse->cursor_shown && !mouse->relative_mode &&
        mouse->MoveCursor && mouse->cur_cursor) {
        mouse->MoveCursor(mouse->cur_cursor);
    }

    /* Post the event, if desired */
    posted = 0;
    if (SDL_GetEventState(SDL_MOUSEMOTION) == SDL_ENABLE) {
        SDL_Event event;
        event.motion.type = SDL_MOUSEMOTION;
        event.motion.windowID = mouse->focus ? mouse->focus->id : 0;
        event.motion.which = mouseID;
        event.motion.state = mouse->buttonstate;
        event.motion.x = mouse->x;
        event.motion.y = mouse->y;
        event.motion.xrel = xrel;
        event.motion.yrel = yrel;
        posted = (SDL_PushEvent(&event) > 0);
    }
    /* Use unclamped values if we're getting events outside the window */
    mouse->last_x = x;
    mouse->last_y = y;
    return posted;
}

int
SDL_SendMouseButton(SDL_Window * window, SDL_MouseID mouseID, Uint8 state, Uint8 button)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    int posted;
    Uint32 type;
    Uint32 buttonstate = mouse->buttonstate;

    /* Figure out which event to perform */
    switch (state) {
    case SDL_PRESSED:
        type = SDL_MOUSEBUTTONDOWN;
        buttonstate |= SDL_BUTTON(button);
        break;
    case SDL_RELEASED:
        type = SDL_MOUSEBUTTONUP;
        buttonstate &= ~SDL_BUTTON(button);
        break;
    default:
        /* Invalid state -- bail */
        return 0;
    }

    /* We do this after calculating buttonstate so button presses gain focus */
    if (window && state == SDL_PRESSED) {
        SDL_UpdateMouseFocus(window, mouse->x, mouse->y, buttonstate);
    }

    if (buttonstate == mouse->buttonstate) {
        /* Ignore this event, no state change */
        return 0;
    }
    mouse->buttonstate = buttonstate;

    /* Post the event, if desired */
    posted = 0;
    if (SDL_GetEventState(type) == SDL_ENABLE) {
        SDL_Event event;
        event.type = type;
        event.button.windowID = mouse->focus ? mouse->focus->id : 0;
        event.button.which = mouseID;
        event.button.state = state;
        event.button.button = button;
        event.button.x = mouse->x;
        event.button.y = mouse->y;
        posted = (SDL_PushEvent(&event) > 0);
    }

    /* We do this after dispatching event so button releases can lose focus */
    if (window && state == SDL_RELEASED) {
        SDL_UpdateMouseFocus(window, mouse->x, mouse->y, buttonstate);
    }

    return posted;
}

int
SDL_SendMouseWheel(SDL_Window * window, SDL_MouseID mouseID, int x, int y)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    int posted;

    if (window) {
        SDL_SetMouseFocus(window);
    }

    if (!x && !y) {
        return 0;
    }

    /* Post the event, if desired */
    posted = 0;
    if (SDL_GetEventState(SDL_MOUSEWHEEL) == SDL_ENABLE) {
        SDL_Event event;
        event.type = SDL_MOUSEWHEEL;
        event.wheel.windowID = mouse->focus ? mouse->focus->id : 0;
        event.wheel.which = mouseID;
        event.wheel.x = x;
        event.wheel.y = y;
        posted = (SDL_PushEvent(&event) > 0);
    }
    return posted;
}

void
SDL_MouseQuit(void)
{
    SDL_Cursor *cursor, *next;
    SDL_Mouse *mouse = SDL_GetMouse();

    SDL_ShowCursor(1);

    cursor = mouse->cursors;
    while (cursor) {
        next = cursor->next;
        SDL_FreeCursor(cursor);
        cursor = next;
    }

    if (mouse->def_cursor && mouse->FreeCursor) {
        mouse->FreeCursor(mouse->def_cursor);
    }

    SDL_zerop(mouse);
}

Uint32
SDL_GetMouseState(int *x, int *y)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (x) {
        *x = mouse->x;
    }
    if (y) {
        *y = mouse->y;
    }
    return mouse->buttonstate;
}

Uint32
SDL_GetRelativeMouseState(int *x, int *y)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (x) {
        *x = mouse->xdelta;
    }
    if (y) {
        *y = mouse->ydelta;
    }
    mouse->xdelta = 0;
    mouse->ydelta = 0;
    return mouse->buttonstate;
}

void
SDL_WarpMouseInWindow(SDL_Window * window, int x, int y)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if ( window == NULL )
        window = mouse->focus;

    if ( window == NULL )
        return;

    if (mouse->WarpMouse) {
        mouse->WarpMouse(window, x, y);
    } else {
        SDL_SendMouseMotion(window, mouse->mouseID, 0, x, y);
    }
}

int
SDL_SetRelativeMouseMode(SDL_bool enabled)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    SDL_Window *focusWindow = SDL_GetKeyboardFocus();
    int original_x = mouse->x, original_y = mouse->y;

    if (enabled == mouse->relative_mode) {
        return 0;
    }

    if (!mouse->SetRelativeMouseMode) {
        return SDL_Unsupported();
    }

    if (enabled && focusWindow) {
        /* Center it in the focused window to prevent clicks from going through
         * to background windows.
         */
        SDL_SetMouseFocus(focusWindow);
        SDL_WarpMouseInWindow(focusWindow, focusWindow->w/2, focusWindow->h/2);
    }

    if (mouse->SetRelativeMouseMode(enabled) < 0) {
        return -1;
    }

    /* Set the relative mode */
    mouse->relative_mode = enabled;

    if (enabled) {
        /* Save the expected mouse position */
        mouse->original_x = original_x;
        mouse->original_y = original_y;
    } else if (mouse->focus) {
        /* Restore the expected mouse position */
        SDL_WarpMouseInWindow(mouse->focus, mouse->original_x, mouse->original_y);
    }

    /* Flush pending mouse motion */
    SDL_FlushEvent(SDL_MOUSEMOTION);

    /* Update cursor visibility */
    SDL_SetCursor(NULL);

    return 0;
}

SDL_bool
SDL_GetRelativeMouseMode()
{
    SDL_Mouse *mouse = SDL_GetMouse();

    return mouse->relative_mode;
}

SDL_Cursor *
SDL_CreateCursor(const Uint8 * data, const Uint8 * mask,
                 int w, int h, int hot_x, int hot_y)
{
    SDL_Surface *surface;
    SDL_Cursor *cursor;
    int x, y;
    Uint32 *pixel;
    Uint8 datab = 0, maskb = 0;
    const Uint32 black = 0xFF000000;
    const Uint32 white = 0xFFFFFFFF;
    const Uint32 transparent = 0x00000000;

    /* Make sure the width is a multiple of 8 */
    w = ((w + 7) & ~7);

    /* Create the surface from a bitmap */
    surface = SDL_CreateRGBSurface(0, w, h, 32,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF,
                                   0xFF000000);
    if (!surface) {
        return NULL;
    }
    for (y = 0; y < h; ++y) {
        pixel = (Uint32 *) ((Uint8 *) surface->pixels + y * surface->pitch);
        for (x = 0; x < w; ++x) {
            if ((x % 8) == 0) {
                datab = *data++;
                maskb = *mask++;
            }
            if (maskb & 0x80) {
                *pixel++ = (datab & 0x80) ? black : white;
            } else {
                *pixel++ = (datab & 0x80) ? black : transparent;
            }
            datab <<= 1;
            maskb <<= 1;
        }
    }

    cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);

    SDL_FreeSurface(surface);

    return cursor;
}

SDL_Cursor *
SDL_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    SDL_Surface *temp = NULL;
    SDL_Cursor *cursor;

    if (!surface) {
        SDL_SetError("Passed NULL cursor surface");
        return NULL;
    }

    if (!mouse->CreateCursor) {
        SDL_SetError("Cursors are not currently supported");
        return NULL;
    }

    /* Sanity check the hot spot */
    if ((hot_x < 0) || (hot_y < 0) ||
        (hot_x >= surface->w) || (hot_y >= surface->h)) {
        SDL_SetError("Cursor hot spot doesn't lie within cursor");
        return NULL;
    }

    if (surface->format->format != SDL_PIXELFORMAT_ARGB8888) {
        temp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        if (!temp) {
            return NULL;
        }
        surface = temp;
    }

    cursor = mouse->CreateCursor(surface, hot_x, hot_y);
    if (cursor) {
        cursor->next = mouse->cursors;
        mouse->cursors = cursor;
    }

    SDL_FreeSurface(temp);

    return cursor;
}

SDL_Cursor *
SDL_CreateSystemCursor(SDL_SystemCursor id)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    SDL_Cursor *cursor;

    if (!mouse->CreateSystemCursor) {
        SDL_SetError("CreateSystemCursor is not currently supported");
        return NULL;
    }

    cursor = mouse->CreateSystemCursor(id);
    if (cursor) {
        cursor->next = mouse->cursors;
        mouse->cursors = cursor;
    }

    return cursor;
}

/* SDL_SetCursor(NULL) can be used to force the cursor redraw,
   if this is desired for any reason.  This is used when setting
   the video mode and when the SDL window gains the mouse focus.
 */
void
SDL_SetCursor(SDL_Cursor * cursor)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    /* Set the new cursor */
    if (cursor) {
        /* Make sure the cursor is still valid for this mouse */
        if (cursor != mouse->def_cursor) {
            SDL_Cursor *found;
            for (found = mouse->cursors; found; found = found->next) {
                if (found == cursor) {
                    break;
                }
            }
            if (!found) {
                SDL_SetError("Cursor not associated with the current mouse");
                return;
            }
        }
        mouse->cur_cursor = cursor;
    } else {
        if (mouse->focus) {
            cursor = mouse->cur_cursor;
        } else {
            cursor = mouse->def_cursor;
        }
    }

    if (cursor && mouse->cursor_shown && !mouse->relative_mode) {
        if (mouse->ShowCursor) {
            mouse->ShowCursor(cursor);
        }
    } else {
        if (mouse->ShowCursor) {
            mouse->ShowCursor(NULL);
        }
    }
}

SDL_Cursor *
SDL_GetCursor(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (!mouse) {
        return NULL;
    }
    return mouse->cur_cursor;
}

SDL_Cursor *
SDL_GetDefaultCursor(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (!mouse) {
        return NULL;
    }
    return mouse->def_cursor;
}

void
SDL_FreeCursor(SDL_Cursor * cursor)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    SDL_Cursor *curr, *prev;

    if (!cursor) {
        return;
    }

    if (cursor == mouse->def_cursor) {
        return;
    }
    if (cursor == mouse->cur_cursor) {
        SDL_SetCursor(mouse->def_cursor);
    }

    for (prev = NULL, curr = mouse->cursors; curr;
         prev = curr, curr = curr->next) {
        if (curr == cursor) {
            if (prev) {
                prev->next = curr->next;
            } else {
                mouse->cursors = curr->next;
            }

            if (mouse->FreeCursor) {
                mouse->FreeCursor(curr);
            }
            return;
        }
    }
}

int
SDL_ShowCursor(int toggle)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    SDL_bool shown;

    if (!mouse) {
        return 0;
    }

    shown = mouse->cursor_shown;
    if (toggle >= 0) {
        if (toggle) {
            mouse->cursor_shown = SDL_TRUE;
        } else {
            mouse->cursor_shown = SDL_FALSE;
        }
        if (mouse->cursor_shown != shown) {
            SDL_SetCursor(NULL);
        }
    }
    return shown;
}

/* vi: set ts=4 sw=4 expandtab: */
