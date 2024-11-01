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

#if SDL_VIDEO_DRIVER_WINDOWS

#include "SDL_assert.h"
#include "SDL_windowsvideo.h"

#include "SDL_mouse_c.h"


HCURSOR SDL_cursor = NULL;


static SDL_Cursor *
WIN_CreateDefaultCursor()
{
    SDL_Cursor *cursor;

    cursor = SDL_calloc(1, sizeof(*cursor));
    if (cursor) {
        cursor->driverdata = LoadCursor(NULL, IDC_ARROW);
    } else {
        SDL_OutOfMemory();
    }

    return cursor;
}

static SDL_Cursor *
WIN_CreateCursor(SDL_Surface * surface, int hot_x, int hot_y)
{
    /* msdn says cursor mask has to be padded out to word alignment. Not sure
        if that means machine word or WORD, but this handles either case. */
    const size_t pad = (sizeof (size_t) * 8);  /* 32 or 64, or whatever. */
    SDL_Cursor *cursor;
    HICON hicon;
    HDC hdc;
    BITMAPV4HEADER bmh;
    LPVOID pixels;
    LPVOID maskbits;
    size_t maskbitslen;
    ICONINFO ii;

    SDL_zero(bmh);
    bmh.bV4Size = sizeof(bmh);
    bmh.bV4Width = surface->w;
    bmh.bV4Height = -surface->h; /* Invert the image */
    bmh.bV4Planes = 1;
    bmh.bV4BitCount = 32;
    bmh.bV4V4Compression = BI_BITFIELDS;
    bmh.bV4AlphaMask = 0xFF000000;
    bmh.bV4RedMask   = 0x00FF0000;
    bmh.bV4GreenMask = 0x0000FF00;
    bmh.bV4BlueMask  = 0x000000FF;

    maskbitslen = ((surface->w + (pad - (surface->w % pad))) / 8) * surface->h;
    maskbits = SDL_stack_alloc(Uint8,maskbitslen);
    if (maskbits == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }

    /* AND the cursor against full bits: no change. We already have alpha. */
    SDL_memset(maskbits, 0xFF, maskbitslen);

    hdc = GetDC(NULL);
    SDL_zero(ii);
    ii.fIcon = FALSE;
    ii.xHotspot = (DWORD)hot_x;
    ii.yHotspot = (DWORD)hot_y;
    ii.hbmColor = CreateDIBSection(hdc, (BITMAPINFO*)&bmh, DIB_RGB_COLORS, &pixels, NULL, 0);
    ii.hbmMask = CreateBitmap(surface->w, surface->h, 1, 1, maskbits);
    ReleaseDC(NULL, hdc);
    SDL_stack_free(maskbits);

    SDL_assert(surface->format->format == SDL_PIXELFORMAT_ARGB8888);
    SDL_assert(surface->pitch == surface->w * 4);
    SDL_memcpy(pixels, surface->pixels, surface->h * surface->pitch);

    hicon = CreateIconIndirect(&ii);

    DeleteObject(ii.hbmColor);
    DeleteObject(ii.hbmMask);

    if (!hicon) {
        WIN_SetError("CreateIconIndirect()");
        return NULL;
    }

    cursor = SDL_calloc(1, sizeof(*cursor));
    if (cursor) {
        cursor->driverdata = hicon;
    } else {
        DestroyIcon(hicon);
        SDL_OutOfMemory();
    }

    return cursor;
}

static SDL_Cursor *
WIN_CreateSystemCursor(SDL_SystemCursor id)
{
    SDL_Cursor *cursor;
    LPCTSTR name;

    switch(id)
    {
    default:
        SDL_assert(0);
        return NULL;
    case SDL_SYSTEM_CURSOR_ARROW:     name = IDC_ARROW; break;
    case SDL_SYSTEM_CURSOR_IBEAM:     name = IDC_IBEAM; break;
    case SDL_SYSTEM_CURSOR_WAIT:      name = IDC_WAIT; break;
    case SDL_SYSTEM_CURSOR_CROSSHAIR: name = IDC_CROSS; break;
    case SDL_SYSTEM_CURSOR_WAITARROW: name = IDC_WAIT; break;
    case SDL_SYSTEM_CURSOR_SIZENWSE:  name = IDC_SIZENWSE; break;
    case SDL_SYSTEM_CURSOR_SIZENESW:  name = IDC_SIZENESW; break;
    case SDL_SYSTEM_CURSOR_SIZEWE:    name = IDC_SIZEWE; break;
    case SDL_SYSTEM_CURSOR_SIZENS:    name = IDC_SIZENS; break;
    case SDL_SYSTEM_CURSOR_SIZEALL:   name = IDC_SIZEALL; break;
    case SDL_SYSTEM_CURSOR_NO:        name = IDC_NO; break;
    case SDL_SYSTEM_CURSOR_HAND:      name = IDC_HAND; break;
    }

    cursor = SDL_calloc(1, sizeof(*cursor));
    if (cursor) {
        HICON hicon;

        hicon = LoadCursor(NULL, name);

        cursor->driverdata = hicon;
    } else {
        SDL_OutOfMemory();
    }

    return cursor;
}

static void
WIN_FreeCursor(SDL_Cursor * cursor)
{
    HICON hicon = (HICON)cursor->driverdata;

    DestroyIcon(hicon);
    SDL_free(cursor);
}

static int
WIN_ShowCursor(SDL_Cursor * cursor)
{
    if (cursor) {
        SDL_cursor = (HCURSOR)cursor->driverdata;
    } else {
        SDL_cursor = NULL;
    }
    if (SDL_GetMouseFocus() != NULL) {
        SetCursor(SDL_cursor);
    }
    return 0;
}

static void
WIN_WarpMouse(SDL_Window * window, int x, int y)
{
    HWND hwnd = ((SDL_WindowData *) window->driverdata)->hwnd;
    POINT pt;

    pt.x = x;
    pt.y = y;
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

static int
WIN_SetRelativeMouseMode(SDL_bool enabled)
{
    RAWINPUTDEVICE rawMouse = { 0x01, 0x02, 0, NULL }; /* Mouse: UsagePage = 1, Usage = 2 */
    HWND hWnd;
    hWnd = GetActiveWindow();

    rawMouse.hwndTarget = hWnd;
    if(!enabled) {
        rawMouse.dwFlags |= RIDEV_REMOVE;
        rawMouse.hwndTarget = NULL;
    }


    /* (Un)register raw input for mice */
    if(RegisterRawInputDevices(&rawMouse, 1, sizeof(RAWINPUTDEVICE)) == FALSE) {

        /* Only return an error when registering. If we unregister and fail, then
        it's probably that we unregistered twice. That's OK. */
        if(enabled) {
            return SDL_Unsupported();
        }
    }

    if(enabled) {
        LONG cx, cy;
        RECT rect;
        GetWindowRect(hWnd, &rect);

        cx = (rect.left + rect.right) / 2;
        cy = (rect.top + rect.bottom) / 2;

        /* Make an absurdly small clip rect */
        rect.left = cx-1;
        rect.right = cx+1;
        rect.top = cy-1;
        rect.bottom = cy+1;

        ClipCursor(&rect);
    }
    else
        ClipCursor(NULL);

    return 0;
}

void
WIN_InitMouse(_THIS)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->CreateCursor = WIN_CreateCursor;
    mouse->CreateSystemCursor = WIN_CreateSystemCursor;
    mouse->ShowCursor = WIN_ShowCursor;
    mouse->FreeCursor = WIN_FreeCursor;
    mouse->WarpMouse = WIN_WarpMouse;
    mouse->SetRelativeMouseMode = WIN_SetRelativeMouseMode;

    SDL_SetDefaultCursor(WIN_CreateDefaultCursor());
}

void
WIN_QuitMouse(_THIS)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    if ( mouse->def_cursor ) {
        SDL_free(mouse->def_cursor);
        mouse->def_cursor = NULL;
        mouse->cur_cursor = NULL;
    }
}

#endif /* SDL_VIDEO_DRIVER_WINDOWS */

/* vi: set ts=4 sw=4 expandtab: */
