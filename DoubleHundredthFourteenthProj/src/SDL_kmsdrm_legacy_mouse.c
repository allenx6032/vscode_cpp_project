/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

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

#if SDL_VIDEO_DRIVER_KMSDRM

#include "SDL_kmsdrm_legacy_video.h"
#include "SDL_kmsdrm_legacy_mouse.h"
#include "SDL_kmsdrm_legacy_dyn.h"

#include "SDL_mouse_c.h"
#include "../../events/default_cursor.h"

static SDL_Cursor *KMSDRM_LEGACY_CreateDefaultCursor(void);
static SDL_Cursor *KMSDRM_LEGACY_CreateCursor(SDL_Surface * surface, int hot_x, int hot_y);
static int KMSDRM_LEGACY_ShowCursor(SDL_Cursor * cursor);
static void KMSDRM_LEGACY_MoveCursor(SDL_Cursor * cursor);
static void KMSDRM_LEGACY_FreeCursor(SDL_Cursor * cursor);
static void KMSDRM_LEGACY_WarpMouse(SDL_Window * window, int x, int y);
static int KMSDRM_LEGACY_WarpMouseGlobal(int x, int y);

static SDL_Cursor *
KMSDRM_LEGACY_CreateDefaultCursor(void)
{
    return SDL_CreateCursor(default_cdata, default_cmask, DEFAULT_CWIDTH, DEFAULT_CHEIGHT, DEFAULT_CHOTX, DEFAULT_CHOTY);
}

/* Evaluate if a given cursor size is supported or not. Notably, current Intel gfx only support 64x64 and up. */
static SDL_bool
KMSDRM_LEGACY_IsCursorSizeSupported (int w, int h, uint32_t bo_format) {

    SDL_VideoDevice *dev = SDL_GetVideoDevice();
    SDL_VideoData *viddata = ((SDL_VideoData *)dev->driverdata);
    SDL_DisplayData *dispdata = (SDL_DisplayData *)SDL_GetDisplayDriverData(0);

    int ret;
    uint32_t bo_handle;
    struct gbm_bo *bo = KMSDRM_LEGACY_gbm_bo_create(viddata->gbm, w, h, bo_format,
                                       GBM_BO_USE_CURSOR | GBM_BO_USE_WRITE);

    if (!bo) {
        SDL_SetError("Could not create GBM cursor BO width size %dx%d for size testing", w, h);
        goto cleanup;
    }

    bo_handle = KMSDRM_LEGACY_gbm_bo_get_handle(bo).u32;
    ret = KMSDRM_LEGACY_drmModeSetCursor(viddata->drm_fd, dispdata->crtc_id, bo_handle, w, h);

    if (ret) {
        goto cleanup;
    }
    else {
        KMSDRM_LEGACY_gbm_bo_destroy(bo);
        return SDL_TRUE;
    }

cleanup:
    if (bo) {
        KMSDRM_LEGACY_gbm_bo_destroy(bo);
    }
    return SDL_FALSE;
}

/* Create a cursor from a surface */
static SDL_Cursor *
KMSDRM_LEGACY_CreateCursor(SDL_Surface * surface, int hot_x, int hot_y)
{
    SDL_VideoDevice *dev = SDL_GetVideoDevice();
    SDL_VideoData *viddata = ((SDL_VideoData *)dev->driverdata);
    SDL_PixelFormat *pixlfmt = surface->format;
    KMSDRM_LEGACY_CursorData *curdata;
    SDL_Cursor *cursor;
    SDL_bool cursor_supported = SDL_FALSE;
    int i, ret, usable_cursor_w, usable_cursor_h;
    uint32_t bo_format, bo_stride;
    char *buffer = NULL;
    size_t bufsize;

    switch(pixlfmt->format) {
    case SDL_PIXELFORMAT_RGB332:
        bo_format = GBM_FORMAT_RGB332;
        break;
    case SDL_PIXELFORMAT_ARGB4444:
        bo_format = GBM_FORMAT_ARGB4444;
        break;
    case SDL_PIXELFORMAT_RGBA4444:
        bo_format = GBM_FORMAT_RGBA4444;
        break;
    case SDL_PIXELFORMAT_ABGR4444:
        bo_format = GBM_FORMAT_ABGR4444;
        break;
    case SDL_PIXELFORMAT_BGRA4444:
        bo_format = GBM_FORMAT_BGRA4444;
        break;
    case SDL_PIXELFORMAT_ARGB1555:
        bo_format = GBM_FORMAT_ARGB1555;
        break;
    case SDL_PIXELFORMAT_RGBA5551:
        bo_format = GBM_FORMAT_RGBA5551;
        break;
    case SDL_PIXELFORMAT_ABGR1555:
        bo_format = GBM_FORMAT_ABGR1555;
        break;
    case SDL_PIXELFORMAT_BGRA5551:
        bo_format = GBM_FORMAT_BGRA5551;
        break;
    case SDL_PIXELFORMAT_RGB565:
        bo_format = GBM_FORMAT_RGB565;
        break;
    case SDL_PIXELFORMAT_BGR565:
        bo_format = GBM_FORMAT_BGR565;
        break;
    case SDL_PIXELFORMAT_RGB888:
    case SDL_PIXELFORMAT_RGB24:
        bo_format = GBM_FORMAT_RGB888;
        break;
    case SDL_PIXELFORMAT_BGR888:
    case SDL_PIXELFORMAT_BGR24:
        bo_format = GBM_FORMAT_BGR888;
        break;
    case SDL_PIXELFORMAT_RGBX8888:
        bo_format = GBM_FORMAT_RGBX8888;
        break;
    case SDL_PIXELFORMAT_BGRX8888:
        bo_format = GBM_FORMAT_BGRX8888;
        break;
    case SDL_PIXELFORMAT_ARGB8888:
        bo_format = GBM_FORMAT_ARGB8888;
        break;
    case SDL_PIXELFORMAT_RGBA8888:
        bo_format = GBM_FORMAT_RGBA8888;
        break;
    case SDL_PIXELFORMAT_ABGR8888:
        bo_format = GBM_FORMAT_ABGR8888;
        break;
    case SDL_PIXELFORMAT_BGRA8888:
        bo_format = GBM_FORMAT_BGRA8888;
        break;
    case SDL_PIXELFORMAT_ARGB2101010:
        bo_format = GBM_FORMAT_ARGB2101010;
        break;
    default:
        SDL_SetError("Unsupported pixel format for cursor");
        return NULL;
    }

    if (!KMSDRM_LEGACY_gbm_device_is_format_supported(viddata->gbm, bo_format, GBM_BO_USE_CURSOR | GBM_BO_USE_WRITE)) {
        SDL_SetError("Unsupported pixel format for cursor");
        return NULL;
    }

    cursor = (SDL_Cursor *) SDL_calloc(1, sizeof(*cursor));
    if (!cursor) {
        SDL_OutOfMemory();
        return NULL;
    }
    curdata = (KMSDRM_LEGACY_CursorData *) SDL_calloc(1, sizeof(*curdata));
    if (!curdata) {
        SDL_OutOfMemory();
        SDL_free(cursor);
        return NULL;
    }

    /* We have to know beforehand if a cursor with the same size as the surface is supported.
     * If it's not, we have to find an usable cursor size and use an intermediate and clean buffer.
     * If we can't find a cursor size supported by the hardware, we won't go on trying to 
     * call SDL_SetCursor() later. */

    usable_cursor_w = surface->w;
    usable_cursor_h = surface->h;

    while (usable_cursor_w <= MAX_CURSOR_W && usable_cursor_h <= MAX_CURSOR_H) { 
        if (KMSDRM_LEGACY_IsCursorSizeSupported(usable_cursor_w, usable_cursor_h, bo_format)) {
            cursor_supported = SDL_TRUE;
            break;
        }
        usable_cursor_w += usable_cursor_w;
        usable_cursor_h += usable_cursor_h;
    }

    if (!cursor_supported) {
        SDL_SetError("Could not find a cursor size supported by the kernel driver");
        goto cleanup;
    }

    curdata->hot_x = hot_x;
    curdata->hot_y = hot_y;
    curdata->w = usable_cursor_w;
    curdata->h = usable_cursor_h;

    curdata->bo = KMSDRM_LEGACY_gbm_bo_create(viddata->gbm, usable_cursor_w, usable_cursor_h, bo_format,
                                       GBM_BO_USE_CURSOR | GBM_BO_USE_WRITE);

    if (!curdata->bo) {
        SDL_SetError("Could not create GBM cursor BO");
        goto cleanup;
    }

    bo_stride = KMSDRM_LEGACY_gbm_bo_get_stride(curdata->bo);
    bufsize = bo_stride * curdata->h;

    if (surface->pitch != bo_stride) {
        /* pitch doesn't match stride, must be copied to temp buffer  */
        buffer = SDL_malloc(bufsize);
        if (!buffer) {
            SDL_OutOfMemory();
            goto cleanup;
        }

        if (SDL_MUSTLOCK(surface)) {
            if (SDL_LockSurface(surface) < 0) {
                /* Could not lock surface */
                goto cleanup;
            }
        }

        /* Clean the whole temporary buffer */
        SDL_memset(buffer, 0x00, bo_stride * curdata->h);

        /* Copy to temporary buffer */
        for (i = 0; i < surface->h; i++) {
            SDL_memcpy(buffer + (i * bo_stride),
                       ((char *)surface->pixels) + (i * surface->pitch),
                       surface->w * pixlfmt->BytesPerPixel);
        }

        if (SDL_MUSTLOCK(surface)) {
            SDL_UnlockSurface(surface);
        }

        if (KMSDRM_LEGACY_gbm_bo_write(curdata->bo, buffer, bufsize)) {
            SDL_SetError("Could not write to GBM cursor BO");
            goto cleanup;
        }

        /* Free temporary buffer */
        SDL_free(buffer);
        buffer = NULL;
    } else {
        /* surface matches BO format */
        if (SDL_MUSTLOCK(surface)) {
            if (SDL_LockSurface(surface) < 0) {
                /* Could not lock surface */
                goto cleanup;
            }
        }

        ret = KMSDRM_LEGACY_gbm_bo_write(curdata->bo, surface->pixels, bufsize);

        if (SDL_MUSTLOCK(surface)) {
            SDL_UnlockSurface(surface);
        }

        if (ret) {
            SDL_SetError("Could not write to GBM cursor BO");
            goto cleanup;
        }
    }

    cursor->driverdata = curdata;

    return cursor;

cleanup:
    if (buffer) {
        SDL_free(buffer);
    }
    if (cursor) {
        SDL_free(cursor);
    }
    if (curdata) {
        if (curdata->bo) {
            KMSDRM_LEGACY_gbm_bo_destroy(curdata->bo);
        }
        SDL_free(curdata);
    }
    return NULL;
}

/* Show the specified cursor, or hide if cursor is NULL */
static int
KMSDRM_LEGACY_ShowCursor(SDL_Cursor * cursor)
{
    SDL_VideoDevice *dev = SDL_GetVideoDevice();
    SDL_VideoData *viddata = ((SDL_VideoData *)dev->driverdata);
    SDL_Mouse *mouse;
    KMSDRM_LEGACY_CursorData *curdata;
    SDL_VideoDisplay *display = NULL;
    SDL_DisplayData *dispdata = NULL;
    int ret;
    uint32_t bo_handle;

    mouse = SDL_GetMouse();
    if (!mouse) {
        return SDL_SetError("No mouse.");
    }

    if (mouse->focus) {
        display = SDL_GetDisplayForWindow(mouse->focus);
        if (display) {
            dispdata = (SDL_DisplayData*) display->driverdata;
        }
    }

    if (!cursor) {
        /* Hide current cursor */
        if (mouse->cur_cursor && mouse->cur_cursor->driverdata) {
            curdata = (KMSDRM_LEGACY_CursorData *) mouse->cur_cursor->driverdata;

            if (curdata->crtc_id != 0) {
                ret = KMSDRM_LEGACY_drmModeSetCursor(viddata->drm_fd, curdata->crtc_id, 0, 0, 0);
                if (ret) {
                    SDL_SetError("Could not hide current cursor with drmModeSetCursor().");
                    return ret;
                }
                /* Mark previous cursor as not-displayed */
                curdata->crtc_id = 0;

                return 0;
            }
        }
        /* otherwise if possible, hide global cursor */
        if (dispdata && dispdata->crtc_id != 0) {
            ret = KMSDRM_LEGACY_drmModeSetCursor(viddata->drm_fd, dispdata->crtc_id, 0, 0, 0);
            if (ret) {
                SDL_SetError("Could not hide display's cursor with drmModeSetCursor().");
                return ret;
            }
            return 0;
        }

        return SDL_SetError("Couldn't find cursor to hide.");
    }
    /* If cursor != NULL, show new cursor on display */
    if (!display) {
        return SDL_SetError("Could not get display for mouse.");
    }
    if (!dispdata) {
        return SDL_SetError("Could not get display driverdata.");
    }

    curdata = (KMSDRM_LEGACY_CursorData *) cursor->driverdata;
    if (!curdata || !curdata->bo) {
        return SDL_SetError("Cursor not initialized properly.");
    }

    bo_handle = KMSDRM_LEGACY_gbm_bo_get_handle(curdata->bo).u32;
    if (curdata->hot_x == 0 && curdata->hot_y == 0) {
        ret = KMSDRM_LEGACY_drmModeSetCursor(viddata->drm_fd, dispdata->crtc_id, bo_handle,
                                      curdata->w, curdata->h);
    } else {
        ret = KMSDRM_LEGACY_drmModeSetCursor2(viddata->drm_fd, dispdata->crtc_id, bo_handle,
                                       curdata->w, curdata->h, curdata->hot_x, curdata->hot_y);
    }
    if (ret) {
        SDL_SetError("drmModeSetCursor failed.");
        return ret;
    }

    curdata->crtc_id = dispdata->crtc_id;

    return 0;
}

/* Free a window manager cursor */
static void
KMSDRM_LEGACY_FreeCursor(SDL_Cursor * cursor)
{
    KMSDRM_LEGACY_CursorData *curdata;
    int drm_fd;

    if (cursor) {
        curdata = (KMSDRM_LEGACY_CursorData *) cursor->driverdata;

        if (curdata) {
            if (curdata->bo) {
                if (curdata->crtc_id != 0) {
                    drm_fd = KMSDRM_LEGACY_gbm_device_get_fd(KMSDRM_LEGACY_gbm_bo_get_device(curdata->bo));
                    /* Hide the cursor if previously shown on a CRTC */
                    KMSDRM_LEGACY_drmModeSetCursor(drm_fd, curdata->crtc_id, 0, 0, 0);
                    curdata->crtc_id = 0;
                }
                KMSDRM_LEGACY_gbm_bo_destroy(curdata->bo);
                curdata->bo = NULL;
            }
            SDL_free(cursor->driverdata);
        }
        SDL_free(cursor);
    }
}

/* Warp the mouse to (x,y) */
static void
KMSDRM_LEGACY_WarpMouse(SDL_Window * window, int x, int y)
{
    /* Only one global/fullscreen window is supported */
    KMSDRM_LEGACY_WarpMouseGlobal(x, y);
}

/* Warp the mouse to (x,y) */
static int
KMSDRM_LEGACY_WarpMouseGlobal(int x, int y)
{
    KMSDRM_LEGACY_CursorData *curdata;
    SDL_Mouse *mouse = SDL_GetMouse();

    if (mouse && mouse->cur_cursor && mouse->cur_cursor->driverdata) {
        /* Update internal mouse position. */
        SDL_SendMouseMotion(mouse->focus, mouse->mouseID, 0, x, y);

        /* And now update the cursor graphic position on screen. */
        curdata = (KMSDRM_LEGACY_CursorData *) mouse->cur_cursor->driverdata;
        if (curdata->bo) {

            if (curdata->crtc_id != 0) {
                int ret, drm_fd;
                drm_fd = KMSDRM_LEGACY_gbm_device_get_fd(KMSDRM_LEGACY_gbm_bo_get_device(curdata->bo));
                ret = KMSDRM_LEGACY_drmModeMoveCursor(drm_fd, curdata->crtc_id, x, y);

                if (ret) {
                    SDL_SetError("drmModeMoveCursor() failed.");
                }

                return ret;
            } else {
                return SDL_SetError("Cursor is not currently shown.");
            }
        } else {
            return SDL_SetError("Cursor not initialized properly.");
        }
    } else {
        return SDL_SetError("No mouse or current cursor.");
    }
}

void
KMSDRM_LEGACY_InitMouse(_THIS)
{
    /* FIXME: Using UDEV it should be possible to scan all mice
     * but there's no point in doing so as there's no multimice support...yet!
     */
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->CreateCursor = KMSDRM_LEGACY_CreateCursor;
    mouse->ShowCursor = KMSDRM_LEGACY_ShowCursor;
    mouse->MoveCursor = KMSDRM_LEGACY_MoveCursor;
    mouse->FreeCursor = KMSDRM_LEGACY_FreeCursor;
    mouse->WarpMouse = KMSDRM_LEGACY_WarpMouse;
    mouse->WarpMouseGlobal = KMSDRM_LEGACY_WarpMouseGlobal;

    SDL_SetDefaultCursor(KMSDRM_LEGACY_CreateDefaultCursor());
}

void
KMSDRM_LEGACY_QuitMouse(_THIS)
{
    /* TODO: ? */
}

/* This is called when a mouse motion event occurs */
static void
KMSDRM_LEGACY_MoveCursor(SDL_Cursor * cursor)
{
    SDL_Mouse *mouse = SDL_GetMouse();
    KMSDRM_LEGACY_CursorData *curdata;
    int drm_fd, ret;

    /* We must NOT call SDL_SendMouseMotion() here or we will enter recursivity!
       That's why we move the cursor graphic ONLY. */
    if (mouse && mouse->cur_cursor && mouse->cur_cursor->driverdata) {
        curdata = (KMSDRM_LEGACY_CursorData *) mouse->cur_cursor->driverdata;
        drm_fd = KMSDRM_LEGACY_gbm_device_get_fd(KMSDRM_LEGACY_gbm_bo_get_device(curdata->bo));
        ret = KMSDRM_LEGACY_drmModeMoveCursor(drm_fd, curdata->crtc_id, mouse->x, mouse->y);

        if (ret) {
            SDL_SetError("drmModeMoveCursor() failed.");
        }
    }
}

#endif /* SDL_VIDEO_DRIVER_KMSDRM */

/* vi: set ts=4 sw=4 expandtab: */
