/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2015 Sam Lantinga <slouken@libsdl.org>

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

#if SDL_AUDIO_DRIVER_WINMM

/* Allow access to a raw mixing buffer */

#include "SDL_windows.h"
#include <mmsystem.h>

#include "SDL_timer.h"
#include "SDL_audio.h"
#include "SDL_audio_c.h"
#include "SDL_winmm.h"

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#define DETECT_DEV_IMPL(iscap, typ, capstyp) \
static void DetectWave##typ##Devs(void) { \
    const UINT iscapture = iscap ? 1 : 0; \
    const UINT devcount = wave##typ##GetNumDevs(); \
    capstyp caps; \
    UINT i; \
    for (i = 0; i < devcount; i++) { \
        if (wave##typ##GetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR) { \
            char *name = WIN_StringToUTF8(caps.szPname); \
            if (name != NULL) { \
                SDL_AddAudioDevice((int) iscapture, name, (void *) ((size_t) i+1)); \
                SDL_free(name); \
            } \
        } \
    } \
}

DETECT_DEV_IMPL(SDL_FALSE, Out, WAVEOUTCAPS)
DETECT_DEV_IMPL(SDL_TRUE, In, WAVEINCAPS)

static void
WINMM_DetectDevices(void)
{
    DetectWaveInDevs();
    DetectWaveOutDevs();
}

static void CALLBACK
CaptureSound(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance,
          DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    SDL_AudioDevice *this = (SDL_AudioDevice *) dwInstance;

    /* Only service "buffer is filled" messages */
    if (uMsg != WIM_DATA)
        return;

    /* Signal that we have a new buffer of data */
    ReleaseSemaphore(this->hidden->audio_sem, 1, NULL);
}


/* The Win32 callback for filling the WAVE device */
static void CALLBACK
FillSound(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,
          DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    SDL_AudioDevice *this = (SDL_AudioDevice *) dwInstance;

    /* Only service "buffer done playing" messages */
    if (uMsg != WOM_DONE)
        return;

    /* Signal that we are done playing a buffer */
    ReleaseSemaphore(this->hidden->audio_sem, 1, NULL);
}

static int
SetMMerror(char *function, MMRESULT code)
{
    int len;
    char errbuf[MAXERRORLENGTH];
    wchar_t werrbuf[MAXERRORLENGTH];

    SDL_snprintf(errbuf, SDL_arraysize(errbuf), "%s: ", function);
    len = SDL_static_cast(int, SDL_strlen(errbuf));

    waveOutGetErrorText(code, werrbuf, MAXERRORLENGTH - len);
    WideCharToMultiByte(CP_ACP, 0, werrbuf, -1, errbuf + len,
                        MAXERRORLENGTH - len, NULL, NULL);

    return SDL_SetError("%s", errbuf);
}

static void
WINMM_WaitDevice(_THIS)
{
    /* Wait for an audio chunk to finish */
    WaitForSingleObject(this->hidden->audio_sem, INFINITE);
}

static Uint8 *
WINMM_GetDeviceBuf(_THIS)
{
    return (Uint8 *) (this->hidden->
                      wavebuf[this->hidden->next_buffer].lpData);
}

static void
WINMM_PlayDevice(_THIS)
{
    /* Queue it up */
    waveOutWrite(this->hidden->hout,
                 &this->hidden->wavebuf[this->hidden->next_buffer],
                 sizeof(this->hidden->wavebuf[0]));
    this->hidden->next_buffer = (this->hidden->next_buffer + 1) % NUM_BUFFERS;
}

static void
WINMM_WaitDone(_THIS)
{
    int i, left;

    do {
        left = NUM_BUFFERS;
        for (i = 0; i < NUM_BUFFERS; ++i) {
            if (this->hidden->wavebuf[i].dwFlags & WHDR_DONE) {
                --left;
            }
        }
        if (left > 0) {
            SDL_Delay(100);
        }
    } while (left > 0);
}

static void
WINMM_CloseDevice(_THIS)
{
    /* Close up audio */
    if (this->hidden != NULL) {
        int i;

        if (this->hidden->audio_sem) {
            CloseHandle(this->hidden->audio_sem);
            this->hidden->audio_sem = 0;
        }

        /* Clean up mixing buffers */
        for (i = 0; i < NUM_BUFFERS; ++i) {
            if (this->hidden->wavebuf[i].dwUser != 0xFFFF) {
                waveOutUnprepareHeader(this->hidden->hout,
                                       &this->hidden->wavebuf[i],
                                       sizeof(this->hidden->wavebuf[i]));
                this->hidden->wavebuf[i].dwUser = 0xFFFF;
            }
        }

        /* Free raw mixing buffer */
        SDL_free(this->hidden->mixbuf);
        this->hidden->mixbuf = NULL;

        if (this->hidden->hin) {
            waveInClose(this->hidden->hin);
            this->hidden->hin = 0;
        }

        if (this->hidden->hout) {
            waveOutClose(this->hidden->hout);
            this->hidden->hout = 0;
        }

        SDL_free(this->hidden);
        this->hidden = NULL;
    }
}

static SDL_bool
PrepWaveFormat(_THIS, UINT devId, WAVEFORMATEX *pfmt, const int iscapture)
{
    SDL_zerop(pfmt);

    if (SDL_AUDIO_ISFLOAT(this->spec.format)) {
        pfmt->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    } else {
        pfmt->wFormatTag = WAVE_FORMAT_PCM;
    }
    pfmt->wBitsPerSample = SDL_AUDIO_BITSIZE(this->spec.format);

    pfmt->nChannels = this->spec.channels;
    pfmt->nSamplesPerSec = this->spec.freq;
    pfmt->nBlockAlign = pfmt->nChannels * (pfmt->wBitsPerSample / 8);
    pfmt->nAvgBytesPerSec = pfmt->nSamplesPerSec * pfmt->nBlockAlign;

    if (iscapture) {
        return (waveInOpen(0, devId, pfmt, 0, 0, WAVE_FORMAT_QUERY) == 0);
    } else {
        return (waveOutOpen(0, devId, pfmt, 0, 0, WAVE_FORMAT_QUERY) == 0);
    }
}

static int
WINMM_OpenDevice(_THIS, void *handle, const char *devname, int iscapture)
{
    SDL_AudioFormat test_format = SDL_FirstAudioFormat(this->spec.format);
    int valid_datatype = 0;
    MMRESULT result;
    WAVEFORMATEX waveformat;
    UINT devId = WAVE_MAPPER;  /* WAVE_MAPPER == choose system's default */
    UINT i;

    if (handle != NULL) {  /* specific device requested? */
        /* -1 because we increment the original value to avoid NULL. */
        const size_t val = ((size_t) handle) - 1;
        devId = (UINT) val;
    }

    /* Initialize all variables that we clean on shutdown */
    this->hidden = (struct SDL_PrivateAudioData *)
        SDL_malloc((sizeof *this->hidden));
    if (this->hidden == NULL) {
        return SDL_OutOfMemory();
    }
    SDL_memset(this->hidden, 0, (sizeof *this->hidden));

    /* Initialize the wavebuf structures for closing */
    for (i = 0; i < NUM_BUFFERS; ++i)
        this->hidden->wavebuf[i].dwUser = 0xFFFF;

    if (this->spec.channels > 2)
        this->spec.channels = 2;        /* !!! FIXME: is this right? */

    while ((!valid_datatype) && (test_format)) {
        switch (test_format) {
        case AUDIO_U8:
        case AUDIO_S16:
        case AUDIO_S32:
        case AUDIO_F32:
            this->spec.format = test_format;
            if (PrepWaveFormat(this, devId, &waveformat, iscapture)) {
                valid_datatype = 1;
            } else {
                test_format = SDL_NextAudioFormat();
            }
            break;

        default:
            test_format = SDL_NextAudioFormat();
            break;
        }
    }

    if (!valid_datatype) {
        WINMM_CloseDevice(this);
        return SDL_SetError("Unsupported audio format");
    }

    /* Update the fragment size as size in bytes */
    SDL_CalculateAudioSpec(&this->spec);

    /* Open the audio device */
    if (iscapture) {
        result = waveInOpen(&this->hidden->hin, devId, &waveformat,
                             (DWORD_PTR) CaptureSound, (DWORD_PTR) this,
                             CALLBACK_FUNCTION);
    } else {
        result = waveOutOpen(&this->hidden->hout, devId, &waveformat,
                             (DWORD_PTR) FillSound, (DWORD_PTR) this,
                             CALLBACK_FUNCTION);
    }

    if (result != MMSYSERR_NOERROR) {
        WINMM_CloseDevice(this);
        return SetMMerror("waveOutOpen()", result);
    }
#ifdef SOUND_DEBUG
    /* Check the sound device we retrieved */
    {
        WAVEOUTCAPS caps;

        result = waveOutGetDevCaps((UINT) this->hidden->hout,
                                   &caps, sizeof(caps));
        if (result != MMSYSERR_NOERROR) {
            WINMM_CloseDevice(this);
            return SetMMerror("waveOutGetDevCaps()", result);
        }
        printf("Audio device: %s\n", caps.szPname);
    }
#endif

    /* Create the audio buffer semaphore */
    this->hidden->audio_sem =
        CreateSemaphore(NULL, NUM_BUFFERS - 1, NUM_BUFFERS, NULL);
    if (this->hidden->audio_sem == NULL) {
        WINMM_CloseDevice(this);
        return SDL_SetError("Couldn't create semaphore");
    }

    /* Create the sound buffers */
    this->hidden->mixbuf =
        (Uint8 *) SDL_malloc(NUM_BUFFERS * this->spec.size);
    if (this->hidden->mixbuf == NULL) {
        WINMM_CloseDevice(this);
        return SDL_OutOfMemory();
    }
    for (i = 0; i < NUM_BUFFERS; ++i) {
        SDL_memset(&this->hidden->wavebuf[i], 0,
                   sizeof(this->hidden->wavebuf[i]));
        this->hidden->wavebuf[i].dwBufferLength = this->spec.size;
        this->hidden->wavebuf[i].dwFlags = WHDR_DONE;
        this->hidden->wavebuf[i].lpData =
            (LPSTR) & this->hidden->mixbuf[i * this->spec.size];
        result = waveOutPrepareHeader(this->hidden->hout,
                                      &this->hidden->wavebuf[i],
                                      sizeof(this->hidden->wavebuf[i]));
        if (result != MMSYSERR_NOERROR) {
            WINMM_CloseDevice(this);
            return SetMMerror("waveOutPrepareHeader()", result);
        }
    }

    return 0;                   /* Ready to go! */
}


static int
WINMM_Init(SDL_AudioDriverImpl * impl)
{
    /* Set the function pointers */
    impl->DetectDevices = WINMM_DetectDevices;
    impl->OpenDevice = WINMM_OpenDevice;
    impl->PlayDevice = WINMM_PlayDevice;
    impl->WaitDevice = WINMM_WaitDevice;
    impl->WaitDone = WINMM_WaitDone;
    impl->GetDeviceBuf = WINMM_GetDeviceBuf;
    impl->CloseDevice = WINMM_CloseDevice;

    return 1;   /* this audio target is available. */
}

AudioBootStrap WINMM_bootstrap = {
    "winmm", "Windows Waveform Audio", WINMM_Init, 0
};

#endif /* SDL_AUDIO_DRIVER_WINMM */

/* vi: set ts=4 sw=4 expandtab: */
