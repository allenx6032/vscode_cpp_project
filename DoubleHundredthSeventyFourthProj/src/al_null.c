/**
 * OpenAL cross platform audio library
 * Copyright (C) 2010 by Chris Robinson
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "al_config.h"

#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h> 
#include "al_alMain.h"
#include "AL_al.h"
#include "AL_alc.h"


typedef struct {
    ALvoid *buffer;
    ALuint size;

    volatile int killNow;
    ALvoid *thread;
} null_data;


static const ALCchar nullDevice[] = "No Output";

static ALuint NullProc(ALvoid *ptr)
{
    ALCdevice *Device = (ALCdevice*)ptr;
    null_data *data = (null_data*)Device->ExtraData;
    ALuint now, start;
    ALuint64 avail, done;
    const ALuint restTime = (ALuint64)Device->UpdateSize * 1000 /
                            Device->Frequency / 2;

    done = 0;
    start = timeGetTime();
    while(!data->killNow && Device->Connected)
    {
        now = timeGetTime();

        avail = (ALuint64)(now-start) * Device->Frequency / 1000;
        if(avail < done)
        {
            /* Timer wrapped. Add the remainder of the cycle to the available
             * count and reset the number of samples done */
            avail += (ALuint64)0xFFFFFFFFu*Device->Frequency/1000 - done;
            done = 0;
        }
        if(avail-done < Device->UpdateSize)
        {
            Sleep(restTime);
            continue;
        }

        while(avail-done >= Device->UpdateSize)
        {
            aluMixData(Device, data->buffer, Device->UpdateSize);
            done += Device->UpdateSize;
        }
    }

    return 0;
}

static ALCboolean null_open_playback(ALCdevice *device, const ALCchar *deviceName)
{
    null_data *data;

    if(!deviceName)
        deviceName = nullDevice;
    else if(strcmp(deviceName, nullDevice) != 0)
        return ALC_FALSE;

    data = (null_data*)calloc(1, sizeof(*data));

    device->szDeviceName = strdup(deviceName);
    device->ExtraData = data;
    return ALC_TRUE;
}

static void null_close_playback(ALCdevice *device)
{
    null_data *data = (null_data*)device->ExtraData;

    free(data);
    device->ExtraData = NULL;
}

static ALCboolean null_reset_playback(ALCdevice *device)
{
    null_data *data = (null_data*)device->ExtraData;

    data->size = device->UpdateSize * FrameSizeFromDevFmt(device->FmtChans,
                                                          device->FmtType);
    data->buffer = malloc(data->size);
    if(!data->buffer)
    {
        AL_PRINT("buffer malloc failed\n");
        return ALC_FALSE;
    }
    SetDefaultWFXChannelOrder(device);

    data->thread = StartThread(NullProc, device);
    if(data->thread == NULL)
    {
        free(data->buffer);
        data->buffer = NULL;
        return ALC_FALSE;
    }

    return ALC_TRUE;
}

static void null_stop_playback(ALCdevice *device)
{
    null_data *data = (null_data*)device->ExtraData;

    if(!data->thread)
        return;

    data->killNow = 1;
    StopThread(data->thread);
    data->thread = NULL;

    data->killNow = 0;

    free(data->buffer);
    data->buffer = NULL;
}


static ALCboolean null_open_capture(ALCdevice *device, const ALCchar *deviceName)
{
    (void)device;
    (void)deviceName;
    return ALC_FALSE;
}


BackendFuncs null_funcs = {
    null_open_playback,
    null_close_playback,
    null_reset_playback,
    null_stop_playback,
    null_open_capture,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

void alc_null_init(BackendFuncs *func_list)
{
    *func_list = null_funcs;
}

void alc_null_deinit(void)
{
}

void alc_null_probe(int type)
{
    if(type == DEVICE_PROBE)
        AppendDeviceList(nullDevice);
    else if(type == ALL_DEVICE_PROBE)
        AppendAllDeviceList(nullDevice);
}
