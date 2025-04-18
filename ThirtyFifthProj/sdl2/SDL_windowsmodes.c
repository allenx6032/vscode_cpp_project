/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

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

#if SDL_VIDEO_DRIVER_WINDOWS

#include "SDL_windowsvideo.h"
#include "SDL_displayevents_c.h"

/* Windows CE compatibility */
#ifndef CDS_FULLSCREEN
#define CDS_FULLSCREEN 0
#endif

/* #define DEBUG_MODES */

static void
WIN_UpdateDisplayMode(_THIS, LPCWSTR deviceName, DWORD index, SDL_DisplayMode * mode)
{
    SDL_DisplayModeData *data = (SDL_DisplayModeData *) mode->driverdata;
    HDC hdc;

    data->DeviceMode.dmFields =
        (DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY |
         DM_DISPLAYFLAGS);

    if (index == ENUM_CURRENT_SETTINGS
        && (hdc = CreateDC(deviceName, NULL, NULL, NULL)) != NULL) {
        char bmi_data[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
        LPBITMAPINFO bmi;
        HBITMAP hbm;
        int logical_width = GetDeviceCaps( hdc, HORZRES );
        int logical_height = GetDeviceCaps( hdc, VERTRES );

        mode->w = logical_width;
        mode->h = logical_height;
        
        SDL_zeroa(bmi_data);
        bmi = (LPBITMAPINFO) bmi_data;
        bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        hbm = CreateCompatibleBitmap(hdc, 1, 1);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        DeleteObject(hbm);
        DeleteDC(hdc);
        if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
            switch (*(Uint32 *) bmi->bmiColors) {
            case 0x00FF0000:
                mode->format = SDL_PIXELFORMAT_RGB888;
                break;
            case 0x000000FF:
                mode->format = SDL_PIXELFORMAT_BGR888;
                break;
            case 0xF800:
                mode->format = SDL_PIXELFORMAT_RGB565;
                break;
            case 0x7C00:
                mode->format = SDL_PIXELFORMAT_RGB555;
                break;
            }
        } else if (bmi->bmiHeader.biBitCount == 8) {
            mode->format = SDL_PIXELFORMAT_INDEX8;
        } else if (bmi->bmiHeader.biBitCount == 4) {
            mode->format = SDL_PIXELFORMAT_INDEX4LSB;
        }
    } else if (mode->format == SDL_PIXELFORMAT_UNKNOWN) {
        /* FIXME: Can we tell what this will be? */
        if ((data->DeviceMode.dmFields & DM_BITSPERPEL) == DM_BITSPERPEL) {
            switch (data->DeviceMode.dmBitsPerPel) {
            case 32:
                mode->format = SDL_PIXELFORMAT_RGB888;
                break;
            case 24:
                mode->format = SDL_PIXELFORMAT_RGB24;
                break;
            case 16:
                mode->format = SDL_PIXELFORMAT_RGB565;
                break;
            case 15:
                mode->format = SDL_PIXELFORMAT_RGB555;
                break;
            case 8:
                mode->format = SDL_PIXELFORMAT_INDEX8;
                break;
            case 4:
                mode->format = SDL_PIXELFORMAT_INDEX4LSB;
                break;
            }
        }
    }
}

static SDL_DisplayOrientation
WIN_GetDisplayOrientation(DEVMODE *mode)
{
    int width = mode->dmPelsWidth;
    int height = mode->dmPelsHeight;

    /* Use unrotated width/height to guess orientation */
    if (mode->dmDisplayOrientation == DMDO_90 || mode->dmDisplayOrientation == DMDO_270) {
        int temp = width;
        width = height;
        height = temp;
    }

    if (width >= height) {
        switch (mode->dmDisplayOrientation) {
        case DMDO_DEFAULT:
            return SDL_ORIENTATION_LANDSCAPE;
        case DMDO_90:
            return SDL_ORIENTATION_PORTRAIT;
        case DMDO_180:
            return SDL_ORIENTATION_LANDSCAPE_FLIPPED;
        case DMDO_270:
            return SDL_ORIENTATION_PORTRAIT_FLIPPED;
        default:
            return SDL_ORIENTATION_UNKNOWN;
        }
    } else {
        switch (mode->dmDisplayOrientation) {
        case DMDO_DEFAULT:
            return SDL_ORIENTATION_PORTRAIT;
        case DMDO_90:
            return SDL_ORIENTATION_LANDSCAPE_FLIPPED;
        case DMDO_180:
            return SDL_ORIENTATION_PORTRAIT_FLIPPED;
        case DMDO_270:
            return SDL_ORIENTATION_LANDSCAPE;
        default:
            return SDL_ORIENTATION_UNKNOWN;
        }
    }
}

static SDL_bool
WIN_GetDisplayMode(_THIS, LPCWSTR deviceName, DWORD index, SDL_DisplayMode * mode, SDL_DisplayOrientation *orientation)
{
    SDL_DisplayModeData *data;
    DEVMODE devmode;

    devmode.dmSize = sizeof(devmode);
    devmode.dmDriverExtra = 0;
    if (!EnumDisplaySettingsW(deviceName, index, &devmode)) {
        return SDL_FALSE;
    }

    data = (SDL_DisplayModeData *) SDL_malloc(sizeof(*data));
    if (!data) {
        return SDL_FALSE;
    }

    mode->driverdata = data;
    data->DeviceMode = devmode;

    mode->format = SDL_PIXELFORMAT_UNKNOWN;
    mode->w = data->DeviceMode.dmPelsWidth;
    mode->h = data->DeviceMode.dmPelsHeight;
    mode->refresh_rate = data->DeviceMode.dmDisplayFrequency;

    /* Fill in the mode information */
    WIN_UpdateDisplayMode(_this, deviceName, index, mode);

    if (orientation) {
        *orientation = WIN_GetDisplayOrientation(&devmode);
    }

    return SDL_TRUE;
}

static SDL_bool
WIN_AddDisplay(_THIS, HMONITOR hMonitor, const MONITORINFOEXW *info, SDL_bool send_event)
{
    int i;
    SDL_VideoDisplay display;
    SDL_DisplayData *displaydata;
    SDL_DisplayMode mode;
    SDL_DisplayOrientation orientation;
    DISPLAY_DEVICEW device;

#ifdef DEBUG_MODES
    SDL_Log("Display: %s\n", WIN_StringToUTF8W(info->szDevice));
#endif

    if (!WIN_GetDisplayMode(_this, info->szDevice, ENUM_CURRENT_SETTINGS, &mode, &orientation)) {
        return SDL_FALSE;
    }

    // Prevent adding duplicate displays. Do this after we know the display is
    // ready to be added to allow any displays that we can't fully query to be
    // removed
    for(i = 0; i < _this->num_displays; ++i) {
        SDL_DisplayData *driverdata = (SDL_DisplayData *)_this->displays[i].driverdata;
        if (SDL_wcscmp(driverdata->DeviceName, info->szDevice) == 0) {
            driverdata->MonitorHandle = hMonitor;
            driverdata->IsValid = SDL_TRUE;

            if (!_this->setting_display_mode) {
                SDL_ResetDisplayModes(i);
                SDL_SetCurrentDisplayMode(&_this->displays[i], &mode);
                SDL_SetDesktopDisplayMode(&_this->displays[i], &mode);
                SDL_SendDisplayEvent(&_this->displays[i], SDL_DISPLAYEVENT_ORIENTATION, orientation);
            }
            return SDL_FALSE;
        }
    }

    displaydata = (SDL_DisplayData *) SDL_malloc(sizeof(*displaydata));
    if (!displaydata) {
        return SDL_FALSE;
    }
    SDL_memcpy(displaydata->DeviceName, info->szDevice,
               sizeof(displaydata->DeviceName));
    displaydata->MonitorHandle = hMonitor;
    displaydata->IsValid = SDL_TRUE;

    SDL_zero(display);
    device.cb = sizeof(device);
    if (EnumDisplayDevicesW(info->szDevice, 0, &device, 0)) {
        display.name = WIN_StringToUTF8W(device.DeviceString);
    }
    display.desktop_mode = mode;
    display.current_mode = mode;
    display.orientation = orientation;
    display.driverdata = displaydata;
    SDL_AddVideoDisplay(&display, send_event);
    SDL_free(display.name);
    return SDL_TRUE;
}

typedef struct _WIN_AddDisplaysData {
    SDL_VideoDevice *video_device;
    SDL_bool send_event;
    SDL_bool want_primary;
} WIN_AddDisplaysData;

static BOOL CALLBACK
WIN_AddDisplaysCallback(HMONITOR hMonitor,
                        HDC      hdcMonitor,
                        LPRECT   lprcMonitor,
                        LPARAM   dwData)
{
    WIN_AddDisplaysData *data = (WIN_AddDisplaysData*)dwData;
    MONITORINFOEXW info;

    SDL_zero(info);
    info.cbSize = sizeof(info);

    if (GetMonitorInfoW(hMonitor, (LPMONITORINFO)&info) != 0) {
        const SDL_bool is_primary = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);

        if (is_primary == data->want_primary) {
            WIN_AddDisplay(data->video_device, hMonitor, &info, data->send_event);
        }
    }

    // continue enumeration
    return TRUE;
}

static void
WIN_AddDisplays(_THIS, SDL_bool send_event)
{
    WIN_AddDisplaysData callback_data;
    callback_data.video_device = _this;
    callback_data.send_event = send_event;

    callback_data.want_primary = SDL_TRUE;
    EnumDisplayMonitors(NULL, NULL, WIN_AddDisplaysCallback, (LPARAM)&callback_data);

    callback_data.want_primary = SDL_FALSE;
    EnumDisplayMonitors(NULL, NULL, WIN_AddDisplaysCallback, (LPARAM)&callback_data);
}

int
WIN_InitModes(_THIS)
{
    WIN_AddDisplays(_this, SDL_FALSE);

    if (_this->num_displays == 0) {
        return SDL_SetError("No displays available");
    }
    return 0;
}

int
WIN_GetDisplayBounds(_THIS, SDL_VideoDisplay * display, SDL_Rect * rect)
{
    const SDL_DisplayData *data = (const SDL_DisplayData *)display->driverdata;
    MONITORINFO minfo;
    BOOL rc;

    SDL_zero(minfo);
    minfo.cbSize = sizeof(MONITORINFO);
    rc = GetMonitorInfo(data->MonitorHandle, &minfo);

    if (!rc) {
        return SDL_SetError("Couldn't find monitor data");
    }

    rect->x = minfo.rcMonitor.left;
    rect->y = minfo.rcMonitor.top;
    rect->w = minfo.rcMonitor.right - minfo.rcMonitor.left;
    rect->h = minfo.rcMonitor.bottom - minfo.rcMonitor.top;

    return 0;
}

int
WIN_GetDisplayDPI(_THIS, SDL_VideoDisplay * display, float * ddpi_out, float * hdpi_out, float * vdpi_out)
{
    const SDL_DisplayData *displaydata = (SDL_DisplayData *)display->driverdata;
    const SDL_VideoData *videodata = (SDL_VideoData *)display->device->driverdata;
    float hdpi = 0, vdpi = 0, ddpi = 0;
    
    if (videodata->GetDpiForMonitor) {
        UINT hdpi_uint, vdpi_uint;
        // Windows 8.1+ codepath
        if (videodata->GetDpiForMonitor(displaydata->MonitorHandle, MDT_EFFECTIVE_DPI, &hdpi_uint, &vdpi_uint) == S_OK) {
            // GetDpiForMonitor docs promise to return the same hdpi/vdpi
            hdpi = (float)hdpi_uint;
            vdpi = (float)hdpi_uint;
            ddpi = (float)hdpi_uint;
        } else {
            return SDL_SetError("GetDpiForMonitor failed");
        }
    } else {
        // Window 8.0 and below: same DPI for all monitors.
        HDC hdc;
        int hdpi_int, vdpi_int, hpoints, vpoints, hpix, vpix;
        float hinches, vinches;

        hdc = GetDC(NULL);
        if (hdc == NULL) {
            return SDL_SetError("GetDC failed");
        }
        hdpi_int = GetDeviceCaps(hdc, LOGPIXELSX);
        vdpi_int = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);

        hpoints = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        vpoints = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        hpix = MulDiv(hpoints, hdpi_int, 96);
        vpix = MulDiv(vpoints, vdpi_int, 96);

        hinches = (float)hpoints / 96.0f;
        vinches = (float)vpoints / 96.0f;

        hdpi = (float)hdpi_int;
        vdpi = (float)vdpi_int;
        ddpi = SDL_ComputeDiagonalDPI(hpix, vpix, hinches, vinches);
    }

    if (ddpi_out) {
        *ddpi_out = ddpi;
    }
    if (hdpi_out) {
        *hdpi_out = hdpi;
    }
    if (vdpi_out) {
        *vdpi_out = vdpi;
    }

    return ddpi != 0.0f ? 0 : SDL_SetError("Couldn't get DPI");
}

int
WIN_GetDisplayUsableBounds(_THIS, SDL_VideoDisplay * display, SDL_Rect * rect)
{
    const SDL_DisplayData *data = (const SDL_DisplayData *)display->driverdata;
    MONITORINFO minfo;
    BOOL rc;

    SDL_zero(minfo);
    minfo.cbSize = sizeof(MONITORINFO);
    rc = GetMonitorInfo(data->MonitorHandle, &minfo);

    if (!rc) {
        return SDL_SetError("Couldn't find monitor data");
    }

    rect->x = minfo.rcWork.left;
    rect->y = minfo.rcWork.top;
    rect->w = minfo.rcWork.right - minfo.rcWork.left;
    rect->h = minfo.rcWork.bottom - minfo.rcWork.top;

    return 0;
}

void
WIN_GetDisplayModes(_THIS, SDL_VideoDisplay * display)
{
    SDL_DisplayData *data = (SDL_DisplayData *) display->driverdata;
    DWORD i;
    SDL_DisplayMode mode;

    for (i = 0; ; ++i) {
        if (!WIN_GetDisplayMode(_this, data->DeviceName, i, &mode, NULL)) {
            break;
        }
        if (SDL_ISPIXELFORMAT_INDEXED(mode.format)) {
            /* We don't support palettized modes now */
            SDL_free(mode.driverdata);
            continue;
        }
        if (mode.format != SDL_PIXELFORMAT_UNKNOWN) {
            if (!SDL_AddDisplayMode(display, &mode)) {
                SDL_free(mode.driverdata);
            }
        } else {
            SDL_free(mode.driverdata);
        }
    }
}

int
WIN_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
    SDL_DisplayData *displaydata = (SDL_DisplayData *) display->driverdata;
    SDL_DisplayModeData *data = (SDL_DisplayModeData *) mode->driverdata;
    LONG status;

    if (mode->driverdata == display->desktop_mode.driverdata) {
        status = ChangeDisplaySettingsExW(displaydata->DeviceName, NULL, NULL, CDS_FULLSCREEN, NULL);
    } else {
        status = ChangeDisplaySettingsExW(displaydata->DeviceName, &data->DeviceMode, NULL, CDS_FULLSCREEN, NULL);
    }
    if (status != DISP_CHANGE_SUCCESSFUL) {
        const char *reason = "Unknown reason";
        switch (status) {
        case DISP_CHANGE_BADFLAGS:
            reason = "DISP_CHANGE_BADFLAGS";
            break;
        case DISP_CHANGE_BADMODE:
            reason = "DISP_CHANGE_BADMODE";
            break;
        case DISP_CHANGE_BADPARAM:
            reason = "DISP_CHANGE_BADPARAM";
            break;
        case DISP_CHANGE_FAILED:
            reason = "DISP_CHANGE_FAILED";
            break;
        }
        return SDL_SetError("ChangeDisplaySettingsEx() failed: %s", reason);
    }
    EnumDisplaySettingsW(displaydata->DeviceName, ENUM_CURRENT_SETTINGS, &data->DeviceMode);
    WIN_UpdateDisplayMode(_this, displaydata->DeviceName, ENUM_CURRENT_SETTINGS, mode);
    return 0;
}

void
WIN_RefreshDisplays(_THIS)
{
    int i;

    // Mark all displays as potentially invalid to detect
    // entries that have actually been removed
    for (i = 0; i < _this->num_displays; ++i) {
        SDL_DisplayData *driverdata = (SDL_DisplayData *)_this->displays[i].driverdata;
        driverdata->IsValid = SDL_FALSE;
    }

    // Enumerate displays to add any new ones and mark still
    // connected entries as valid
    WIN_AddDisplays(_this, SDL_TRUE);

    // Delete any entries still marked as invalid, iterate
    // in reverse as each delete takes effect immediately
    for (i = _this->num_displays - 1; i >= 0; --i) {
        SDL_DisplayData *driverdata = (SDL_DisplayData *)_this->displays[i].driverdata;
        if (driverdata->IsValid == SDL_FALSE) {
            SDL_DelVideoDisplay(i);
        }
    }
}

void
WIN_QuitModes(_THIS)
{
    /* All fullscreen windows should have restored modes by now */
}

#endif /* SDL_VIDEO_DRIVER_WINDOWS */

/* vi: set ts=4 sw=4 expandtab: */
