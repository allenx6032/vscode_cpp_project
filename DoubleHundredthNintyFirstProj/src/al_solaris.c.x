/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include "al_alMain.h"
#include "AL_al.h"
#include "AL_alc.h"

#include <sys/audioio.h>


static const ALCchar solaris_device[] = "Solaris Default";

typedef struct {
    int fd;
    volatile int killNow;
    ALvoid *thread;

    ALubyte *mix_data;
    int data_size;
} solaris_data;


static ALuint SolarisProc(ALvoid *ptr)
{
    ALCdevice *pDevice = (ALCdevice*)ptr;
    solaris_data *data = (solaris_data*)pDevice->ExtraData;
    ALint frameSize;
    int wrote;

    SetRTPriority();

    frameSize = FrameSizeFromDevFmt(pDevice->FmtChans, pDevice->FmtType);

    while(!data->killNow && pDevice->Connected)
    {
        ALint len = data->data_size;
        ALubyte *WritePtr = data->mix_data;

        aluMixData(pDevice, WritePtr, len/frameSize);
        while(len > 0 && !data->killNow)
        {
            wrote = write(data->fd, WritePtr, len);
            if(wrote < 0)
            {
                if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
                {
                    AL_PRINT("write failed: %s\n", strerror(errno));
                    aluHandleDisconnect(pDevice);
                    break;
                }

                Sleep(1);
                continue;
            }

            len -= wrote;
            WritePtr += wrote;
        }
    }

    return 0;
}


static ALCboolean solaris_open_playback(ALCdevice *device, const ALCchar *deviceName)
{
    char driver[64];
    solaris_data *data;

    strncpy(driver, GetConfigValue("solaris", "device", "/dev/audio"), sizeof(driver)-1);
    driver[sizeof(driver)-1] = 0;

    if(!deviceName)
        deviceName = solaris_device;
    else if(strcmp(deviceName, solaris_device) != 0)
        return ALC_FALSE;

    data = (solaris_data*)calloc(1, sizeof(solaris_data));
    data->killNow = 0;

    data->fd = open(driver, O_WRONLY);
    if(data->fd == -1)
    {
        free(data);
        AL_PRINT("Could not open %s: %s\n", driver, strerror(errno));
        return ALC_FALSE;
    }

    device->szDeviceName = strdup(deviceName);
    device->ExtraData = data;
    return ALC_TRUE;
}

static void solaris_close_playback(ALCdevice *device)
{
    solaris_data *data = (solaris_data*)device->ExtraData;

    close(data->fd);
    free(data);
    device->ExtraData = NULL;
}

static ALCboolean solaris_reset_playback(ALCdevice *device)
{
    solaris_data *data = (solaris_data*)device->ExtraData;
    audio_info_t info;
    ALuint frameSize;
    int numChannels;

    AUDIO_INITINFO(&info);

    info.play.sample_rate = device->Frequency;

    if(device->FmtChans != DevFmtMono)
        device->FmtChans = DevFmtStereo;
    numChannels = ChannelsFromDevFmt(device->FmtChans);
    info.play.channels = numChannels;

    switch(device->FmtType)
    {
        case DevFmtByte:
            info.play.precision = 8;
            info.play.encoding = AUDIO_ENCODING_LINEAR;
            break;
        case DevFmtUByte:
            info.play.precision = 8;
            info.play.encoding = AUDIO_ENCODING_LINEAR8;
            break;
        case DevFmtUShort:
        case DevFmtFloat:
            device->FmtType = DevFmtShort;
            /* fall-through */
        case DevFmtShort:
            info.play.precision = 16;
            info.play.encoding = AUDIO_ENCODING_LINEAR;
            break;
    }

    frameSize = numChannels * BytesFromDevFmt(device->FmtType);
    info.play.buffer_size = device->UpdateSize*device->NumUpdates * frameSize;

    if(ioctl(data->fd, AUDIO_SETINFO, &info) < 0)
    {
        AL_PRINT("ioctl failed: %s\n", strerror(errno));
        return ALC_FALSE;
    }

    if(ChannelsFromDevFmt(device->FmtChans) != info.play.channels)
    {
        AL_PRINT("Could not set %d channels, got %d instead\n", ChannelsFromDevFmt(device->FmtChans), info.play.channels);
        return ALC_FALSE;
    }

    if(!((info.play.precision == 8 && info.play.encoding == AUDIO_ENCODING_LINEAR &&
          device->FmtType == DevFmtByte) ||
         (info.play.precision == 8 && info.play.encoding == AUDIO_ENCODING_LINEAR8 &&
          device->FmtType == DevFmtUByte) ||
         (info.play.precision == 16 && info.play.encoding == AUDIO_ENCODING_LINEAR &&
          device->FmtType == DevFmtShort)))
    {
        AL_PRINT("Could not set %#x sample type, got %d (%#x)\n",
                 device->FmtType, info.play.precision, info.play.encoding);
        return ALC_FALSE;
    }

    device->Frequency = info.play.sample_rate;
    device->UpdateSize = (info.play.buffer_size/device->NumUpdates) + 1;

    data->data_size = device->UpdateSize * frameSize;
    data->mix_data = calloc(1, data->data_size);

    SetDefaultChannelOrder(device);

    data->thread = StartThread(SolarisProc, device);
    if(data->thread == NULL)
    {
        free(data->mix_data);
        data->mix_data = NULL;
        return ALC_FALSE;
    }

    return ALC_TRUE;
}

static void solaris_stop_playback(ALCdevice *device)
{
    solaris_data *data = (solaris_data*)device->ExtraData;

    if(!data->thread)
        return;

    data->killNow = 1;
    StopThread(data->thread);
    data->thread = NULL;

    data->killNow = 0;
    if(ioctl(data->fd, AUDIO_DRAIN) < 0)
        AL_PRINT("Error draining device: %s\n", strerror(errno));

    free(data->mix_data);
    data->mix_data = NULL;
}


static ALCboolean solaris_open_capture(ALCdevice *device, const ALCchar *deviceName)
{
    (void)device;
    (void)deviceName;
    return ALC_FALSE;
}

static void solaris_close_capture(ALCdevice *device)
{
    (void)device;
}

static void solaris_start_capture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void solaris_stop_capture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void solaris_capture_samples(ALCdevice *pDevice, ALCvoid *pBuffer, ALCuint lSamples)
{
    (void)pDevice;
    (void)pBuffer;
    (void)lSamples;
}

static ALCuint solaris_available_samples(ALCdevice *pDevice)
{
    (void)pDevice;
    return 0;
}


BackendFuncs solaris_funcs = {
    solaris_open_playback,
    solaris_close_playback,
    solaris_reset_playback,
    solaris_stop_playback,
    solaris_open_capture,
    solaris_close_capture,
    solaris_start_capture,
    solaris_stop_capture,
    solaris_capture_samples,
    solaris_available_samples
};

void alc_solaris_init(BackendFuncs *func_list)
{
    *func_list = solaris_funcs;
}

void alc_solaris_deinit(void)
{
}

void alc_solaris_probe(int type)
{
#ifdef HAVE_STAT
    struct stat buf;
    if(stat(GetConfigValue("solaris", "device", "/dev/audio"), &buf) != 0)
        return;
#endif

    if(type == DEVICE_PROBE)
        AppendDeviceList(solaris_device);
    else if(type == ALL_DEVICE_PROBE)
        AppendAllDeviceList(solaris_device);
}
