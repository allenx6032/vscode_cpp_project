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

/* Allow access to a raw mixing buffer */

#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_audio_c.h"
#include "SDL_audiomem.h"
#include "SDL_sysaudio.h"

#define _THIS SDL_AudioDevice *_this

static SDL_AudioDriver current_audio;
static SDL_AudioDevice *open_devices[16];

/* !!! FIXME: These are wordy and unlocalized... */
#define DEFAULT_OUTPUT_DEVNAME "System audio output device"
#define DEFAULT_INPUT_DEVNAME "System audio capture device"


/*
 * Not all of these will be compiled and linked in, but it's convenient
 *  to have a complete list here and saves yet-another block of #ifdefs...
 *  Please see bootstrap[], below, for the actual #ifdef mess.
 */
extern AudioBootStrap BSD_AUDIO_bootstrap;
extern AudioBootStrap DSP_bootstrap;
extern AudioBootStrap ALSA_bootstrap;
extern AudioBootStrap PULSEAUDIO_bootstrap;
extern AudioBootStrap QSAAUDIO_bootstrap;
extern AudioBootStrap SUNAUDIO_bootstrap;
extern AudioBootStrap ARTS_bootstrap;
extern AudioBootStrap ESD_bootstrap;
extern AudioBootStrap NACLAUD_bootstrap;
extern AudioBootStrap NAS_bootstrap;
extern AudioBootStrap XAUDIO2_bootstrap;
extern AudioBootStrap DSOUND_bootstrap;
extern AudioBootStrap WINMM_bootstrap;
extern AudioBootStrap PAUDIO_bootstrap;
extern AudioBootStrap HAIKUAUDIO_bootstrap;
extern AudioBootStrap COREAUDIO_bootstrap;
extern AudioBootStrap SNDMGR_bootstrap;
extern AudioBootStrap DISKAUD_bootstrap;
extern AudioBootStrap DUMMYAUD_bootstrap;
extern AudioBootStrap DCAUD_bootstrap;
extern AudioBootStrap DART_bootstrap;
extern AudioBootStrap NDSAUD_bootstrap;
extern AudioBootStrap FUSIONSOUND_bootstrap;
extern AudioBootStrap ANDROIDAUD_bootstrap;
extern AudioBootStrap PSPAUD_bootstrap;
extern AudioBootStrap SNDIO_bootstrap;
extern AudioBootStrap EmscriptenAudio_bootstrap;


/* Available audio drivers */
static const AudioBootStrap *const bootstrap[] = {
#if SDL_AUDIO_DRIVER_PULSEAUDIO
    &PULSEAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ALSA
    &ALSA_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SNDIO
    &SNDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_BSD
    &BSD_AUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_OSS
    &DSP_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_QSA
    &QSAAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SUNAUDIO
    &SUNAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ARTS
    &ARTS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ESD
    &ESD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_NACL
   &NACLAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_NAS
    &NAS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_XAUDIO2
    &XAUDIO2_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DSOUND
    &DSOUND_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_WINMM
    &WINMM_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_PAUDIO
    &PAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_HAIKU
    &HAIKUAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_COREAUDIO
    &COREAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DISK
    &DISKAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DUMMY
    &DUMMYAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_FUSIONSOUND
    &FUSIONSOUND_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ANDROID
    &ANDROIDAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_PSP
    &PSPAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_EMSCRIPTEN
    &EmscriptenAudio_bootstrap,
#endif
    NULL
};

static SDL_AudioDevice *
get_audio_device(SDL_AudioDeviceID id)
{
    id--;
    if ((id >= SDL_arraysize(open_devices)) || (open_devices[id] == NULL)) {
        SDL_SetError("Invalid audio device ID");
        return NULL;
    }

    return open_devices[id];
}


/* stubs for audio drivers that don't need a specific entry point... */
static void
SDL_AudioDetectDevices_Default(void)
{
    /* you have to write your own implementation if these assertions fail. */
    SDL_assert(current_audio.impl.OnlyHasDefaultOutputDevice);
    SDL_assert(current_audio.impl.OnlyHasDefaultInputDevice || !current_audio.impl.HasCaptureSupport);

    SDL_AddAudioDevice(SDL_FALSE, DEFAULT_OUTPUT_DEVNAME, (void *) ((size_t) 0x1));
    if (current_audio.impl.HasCaptureSupport) {
        SDL_AddAudioDevice(SDL_TRUE, DEFAULT_INPUT_DEVNAME, (void *) ((size_t) 0x2));
    }
}

static void
SDL_AudioThreadInit_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioWaitDevice_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioPlayDevice_Default(_THIS)
{                               /* no-op. */
}

static int
SDL_AudioGetPendingBytes_Default(_THIS)
{
    return 0;
}

static Uint8 *
SDL_AudioGetDeviceBuf_Default(_THIS)
{
    return NULL;
}

static void
SDL_AudioWaitDone_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioCloseDevice_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioDeinitialize_Default(void)
{                               /* no-op. */
}

static void
SDL_AudioFreeDeviceHandle_Default(void *handle)
{                               /* no-op. */
}


static int
SDL_AudioOpenDevice_Default(_THIS, void *handle, const char *devname, int iscapture)
{
    return SDL_Unsupported();
}

static SDL_INLINE SDL_bool
is_in_audio_device_thread(SDL_AudioDevice * device)
{
    /* The device thread locks the same mutex, but not through the public API.
       This check is in case the application, in the audio callback,
       tries to lock the thread that we've already locked from the
       device thread...just in case we only have non-recursive mutexes. */
    if (device->thread && (SDL_ThreadID() == device->threadid)) {
        return SDL_TRUE;
    }

    return SDL_FALSE;
}

static void
SDL_AudioLockDevice_Default(SDL_AudioDevice * device)
{
    if (!is_in_audio_device_thread(device)) {
        SDL_LockMutex(device->mixer_lock);
    }
}

static void
SDL_AudioUnlockDevice_Default(SDL_AudioDevice * device)
{
    if (!is_in_audio_device_thread(device)) {
        SDL_UnlockMutex(device->mixer_lock);
    }
}


static void
finalize_audio_entry_points(void)
{
    /*
     * Fill in stub functions for unused driver entry points. This lets us
     *  blindly call them without having to check for validity first.
     */

#define FILL_STUB(x) \
        if (current_audio.impl.x == NULL) { \
            current_audio.impl.x = SDL_Audio##x##_Default; \
        }
    FILL_STUB(DetectDevices);
    FILL_STUB(OpenDevice);
    FILL_STUB(ThreadInit);
    FILL_STUB(WaitDevice);
    FILL_STUB(PlayDevice);
    FILL_STUB(GetPendingBytes);
    FILL_STUB(GetDeviceBuf);
    FILL_STUB(WaitDone);
    FILL_STUB(CloseDevice);
    FILL_STUB(LockDevice);
    FILL_STUB(UnlockDevice);
    FILL_STUB(FreeDeviceHandle);
    FILL_STUB(Deinitialize);
#undef FILL_STUB
}


/* device hotplug support... */

static int
add_audio_device(const char *name, void *handle, SDL_AudioDeviceItem **devices, int *devCount)
{
    int retval = -1;
    const size_t size = sizeof (SDL_AudioDeviceItem) + SDL_strlen(name) + 1;
    SDL_AudioDeviceItem *item = (SDL_AudioDeviceItem *) SDL_malloc(size);
    if (item == NULL) {
        return -1;
    }

    SDL_assert(handle != NULL);  /* we reserve NULL, audio backends can't use it. */

    item->handle = handle;
    SDL_strlcpy(item->name, name, size - sizeof (SDL_AudioDeviceItem));

    SDL_LockMutex(current_audio.detectionLock);
    item->next = *devices;
    *devices = item;
    retval = (*devCount)++;
    SDL_UnlockMutex(current_audio.detectionLock);

    return retval;
}

static SDL_INLINE int
add_capture_device(const char *name, void *handle)
{
    /* !!! FIXME: add this later. SDL_assert(current_audio.impl.HasCaptureSupport);*/
    return add_audio_device(name, handle, &current_audio.inputDevices, &current_audio.inputDeviceCount);
}

static SDL_INLINE int
add_output_device(const char *name, void *handle)
{
    return add_audio_device(name, handle, &current_audio.outputDevices, &current_audio.outputDeviceCount);
}

static void
free_device_list(SDL_AudioDeviceItem **devices, int *devCount)
{
    SDL_AudioDeviceItem *item, *next;
    for (item = *devices; item != NULL; item = next) {
        next = item->next;
        if (item->handle != NULL) {
            current_audio.impl.FreeDeviceHandle(item->handle);
        }
        SDL_free(item);
    }
    *devices = NULL;
    *devCount = 0;
}


/* The audio backends call this when a new device is plugged in. */
void
SDL_AddAudioDevice(const int iscapture, const char *name, void *handle)
{
    const int device_index = iscapture ? add_capture_device(name, handle) : add_output_device(name, handle);
    if (device_index != -1) {
        /* Post the event, if desired */
        if (SDL_GetEventState(SDL_AUDIODEVICEADDED) == SDL_ENABLE) {
            SDL_Event event;
            SDL_zero(event);
            event.adevice.type = SDL_AUDIODEVICEADDED;
            event.adevice.which = device_index;
            event.adevice.iscapture = iscapture;
            SDL_PushEvent(&event);
        }
    }
}

/* The audio backends call this when a currently-opened device is lost. */
void SDL_OpenedAudioDeviceDisconnected(SDL_AudioDevice *device)
{
    SDL_assert(get_audio_device(device->id) == device);

    if (!device->enabled) {
        return;
    }

    /* Ends the audio callback and mark the device as STOPPED, but the
       app still needs to close the device to free resources. */
    current_audio.impl.LockDevice(device);
    device->enabled = 0;
    current_audio.impl.UnlockDevice(device);

    /* Post the event, if desired */
    if (SDL_GetEventState(SDL_AUDIODEVICEREMOVED) == SDL_ENABLE) {
        SDL_Event event;
        SDL_zero(event);
        event.adevice.type = SDL_AUDIODEVICEREMOVED;
        event.adevice.which = device->id;
        event.adevice.iscapture = device->iscapture ? 1 : 0;
        SDL_PushEvent(&event);
    }
}

static void
mark_device_removed(void *handle, SDL_AudioDeviceItem *devices, SDL_bool *removedFlag)
{
    SDL_AudioDeviceItem *item;
    SDL_assert(handle != NULL);
    for (item = devices; item != NULL; item = item->next) {
        if (item->handle == handle) {
            item->handle = NULL;
            *removedFlag = SDL_TRUE;
            return;
        }
    }
}

/* The audio backends call this when a device is removed from the system. */
void
SDL_RemoveAudioDevice(const int iscapture, void *handle)
{
    SDL_LockMutex(current_audio.detectionLock);
    if (iscapture) {
        mark_device_removed(handle, current_audio.inputDevices, &current_audio.captureDevicesRemoved);
    } else {
        mark_device_removed(handle, current_audio.outputDevices, &current_audio.outputDevicesRemoved);
    }
    SDL_UnlockMutex(current_audio.detectionLock);
    current_audio.impl.FreeDeviceHandle(handle);
}



/* buffer queueing support... */

/* this expects that you managed thread safety elsewhere. */
static void
free_audio_queue(SDL_AudioBufferQueue *buffer)
{
    while (buffer) {
        SDL_AudioBufferQueue *next = buffer->next;
        SDL_free(buffer);
        buffer = next;
    }
}

static void SDLCALL
SDL_BufferQueueDrainCallback(void *userdata, Uint8 *stream, int _len)
{
    /* this function always holds the mixer lock before being called. */
    Uint32 len = (Uint32) _len;
    SDL_AudioDevice *device = (SDL_AudioDevice *) userdata;
    SDL_AudioBufferQueue *buffer;

    SDL_assert(device != NULL);  /* this shouldn't ever happen, right?! */
    SDL_assert(_len >= 0);  /* this shouldn't ever happen, right?! */

    while ((len > 0) && ((buffer = device->buffer_queue_head) != NULL)) {
        const Uint32 avail = buffer->datalen - buffer->startpos;
        const Uint32 cpy = SDL_min(len, avail);
        SDL_assert(device->queued_bytes >= avail);

        SDL_memcpy(stream, buffer->data + buffer->startpos, cpy);
        buffer->startpos += cpy;
        stream += cpy;
        device->queued_bytes -= cpy;
        len -= cpy;

        if (buffer->startpos == buffer->datalen) {  /* packet is done, put it in the pool. */
            device->buffer_queue_head = buffer->next;
            SDL_assert((buffer->next != NULL) || (buffer == device->buffer_queue_tail));
            buffer->next = device->buffer_queue_pool;
            device->buffer_queue_pool = buffer;
        }
    }

    SDL_assert((device->buffer_queue_head != NULL) == (device->queued_bytes != 0));

    if (len > 0) {  /* fill any remaining space in the stream with silence. */
        SDL_assert(device->buffer_queue_head == NULL);
        SDL_memset(stream, device->spec.silence, len);
    }

    if (device->buffer_queue_head == NULL) {
        device->buffer_queue_tail = NULL;  /* in case we drained the queue entirely. */
    }
}

int
SDL_QueueAudio(SDL_AudioDeviceID devid, const void *_data, Uint32 len)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    const Uint8 *data = (const Uint8 *) _data;
    SDL_AudioBufferQueue *orighead;
    SDL_AudioBufferQueue *origtail;
    Uint32 origlen;
    Uint32 datalen;

    if (!device) {
        return -1;  /* get_audio_device() will have set the error state */
    }

    if (device->spec.callback != SDL_BufferQueueDrainCallback) {
        return SDL_SetError("Audio device has a callback, queueing not allowed");
    }

    current_audio.impl.LockDevice(device);

    orighead = device->buffer_queue_head;
    origtail = device->buffer_queue_tail;
    origlen = origtail ? origtail->datalen : 0;

    while (len > 0) {
        SDL_AudioBufferQueue *packet = device->buffer_queue_tail;
        SDL_assert(!packet || (packet->datalen <= SDL_AUDIOBUFFERQUEUE_PACKETLEN));
        if (!packet || (packet->datalen >= SDL_AUDIOBUFFERQUEUE_PACKETLEN)) {
            /* tail packet missing or completely full; we need a new packet. */
            packet = device->buffer_queue_pool;
            if (packet != NULL) {
                /* we have one available in the pool. */
                device->buffer_queue_pool = packet->next;
            } else {
                /* Have to allocate a new one! */
                packet = (SDL_AudioBufferQueue *) SDL_malloc(sizeof (SDL_AudioBufferQueue));
                if (packet == NULL) {
                    /* uhoh, reset so we've queued nothing new, free what we can. */
                    if (!origtail) {
                        packet = device->buffer_queue_head;  /* whole queue. */
                    } else {
                        packet = origtail->next;  /* what we added to existing queue. */
                        origtail->next = NULL;
                        origtail->datalen = origlen;
                    }
                    device->buffer_queue_head = orighead;
                    device->buffer_queue_tail = origtail;
                    device->buffer_queue_pool = NULL;

                    current_audio.impl.UnlockDevice(device);

                    free_audio_queue(packet);  /* give back what we can. */

                    return SDL_OutOfMemory();
                }
            }
            packet->datalen = 0;
            packet->startpos = 0;
            packet->next = NULL;

            SDL_assert((device->buffer_queue_head != NULL) == (device->queued_bytes != 0));
            if (device->buffer_queue_tail == NULL) {
                device->buffer_queue_head = packet;
            } else {
                device->buffer_queue_tail->next = packet;
            }
            device->buffer_queue_tail = packet;
        }

        datalen = SDL_min(len, SDL_AUDIOBUFFERQUEUE_PACKETLEN - packet->datalen);
        SDL_memcpy(packet->data + packet->datalen, data, datalen);
        data += datalen;
        len -= datalen;
        packet->datalen += datalen;
        device->queued_bytes += datalen;
    }

    current_audio.impl.UnlockDevice(device);

    return 0;
}

Uint32
SDL_GetQueuedAudioSize(SDL_AudioDeviceID devid)
{
    Uint32 retval = 0;
    SDL_AudioDevice *device = get_audio_device(devid);

    /* Nothing to do unless we're set up for queueing. */
    if (device && (device->spec.callback == SDL_BufferQueueDrainCallback)) {
        current_audio.impl.LockDevice(device);
        retval = device->queued_bytes + current_audio.impl.GetPendingBytes(device);
        current_audio.impl.UnlockDevice(device);
    }

    return retval;
}

void
SDL_ClearQueuedAudio(SDL_AudioDeviceID devid)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    SDL_AudioBufferQueue *buffer = NULL;
    if (!device) {
        return;  /* nothing to do. */
    }

    /* Blank out the device and release the mutex. Free it afterwards. */
    current_audio.impl.LockDevice(device);
    buffer = device->buffer_queue_head;
    device->buffer_queue_tail = NULL;
    device->buffer_queue_head = NULL;
    device->queued_bytes = 0;
    current_audio.impl.UnlockDevice(device);

    free_audio_queue(buffer);
}


/* The general mixing thread function */
int SDLCALL
SDL_RunAudio(void *devicep)
{
    SDL_AudioDevice *device = (SDL_AudioDevice *) devicep;
    const int silence = (int) device->spec.silence;
    const Uint32 delay = ((device->spec.samples * 1000) / device->spec.freq);
    const int stream_len = (device->convert.needed) ? device->convert.len : device->spec.size;
    Uint8 *stream;
    void *udata = device->spec.userdata;
    void (SDLCALL *fill) (void *, Uint8 *, int) = device->spec.callback;

    /* The audio mixing is always a high priority thread */
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    /* Perform any thread setup */
    device->threadid = SDL_ThreadID();
    current_audio.impl.ThreadInit(device);

    /* Loop, filling the audio buffers */
    while (!device->shutdown) {
        /* Fill the current buffer with sound */
        if (device->convert.needed) {
            stream = device->convert.buf;
        } else if (device->enabled) {
            stream = current_audio.impl.GetDeviceBuf(device);
        } else {
            /* if the device isn't enabled, we still write to the
               fake_stream, so the app's callback will fire with
               a regular frequency, in case they depend on that
               for timing or progress. They can use hotplug
               now to know if the device failed. */
            stream = NULL;
        }

        if (stream == NULL) {
            stream = device->fake_stream;
        }

        /* !!! FIXME: this should be LockDevice. */
        SDL_LockMutex(device->mixer_lock);
        if (device->paused) {
            SDL_memset(stream, silence, stream_len);
        } else {
            (*fill) (udata, stream, stream_len);
        }
        SDL_UnlockMutex(device->mixer_lock);

        /* Convert the audio if necessary */
        if (device->enabled && device->convert.needed) {
            SDL_ConvertAudio(&device->convert);
            stream = current_audio.impl.GetDeviceBuf(device);
            if (stream == NULL) {
                stream = device->fake_stream;
            } else {
                SDL_memcpy(stream, device->convert.buf,
                           device->convert.len_cvt);
            }
        }

        /* Ready current buffer for play and change current buffer */
        if (stream == device->fake_stream) {
            SDL_Delay(delay);
        } else {
            current_audio.impl.PlayDevice(device);
            current_audio.impl.WaitDevice(device);
        }
    }

    /* Wait for the audio to drain. */
    current_audio.impl.WaitDone(device);

    return 0;
}


static SDL_AudioFormat
SDL_ParseAudioFormat(const char *string)
{
#define CHECK_FMT_STRING(x) if (SDL_strcmp(string, #x) == 0) return AUDIO_##x
    CHECK_FMT_STRING(U8);
    CHECK_FMT_STRING(S8);
    CHECK_FMT_STRING(U16LSB);
    CHECK_FMT_STRING(S16LSB);
    CHECK_FMT_STRING(U16MSB);
    CHECK_FMT_STRING(S16MSB);
    CHECK_FMT_STRING(U16SYS);
    CHECK_FMT_STRING(S16SYS);
    CHECK_FMT_STRING(U16);
    CHECK_FMT_STRING(S16);
    CHECK_FMT_STRING(S32LSB);
    CHECK_FMT_STRING(S32MSB);
    CHECK_FMT_STRING(S32SYS);
    CHECK_FMT_STRING(S32);
    CHECK_FMT_STRING(F32LSB);
    CHECK_FMT_STRING(F32MSB);
    CHECK_FMT_STRING(F32SYS);
    CHECK_FMT_STRING(F32);
#undef CHECK_FMT_STRING
    return 0;
}

int
SDL_GetNumAudioDrivers(void)
{
    return SDL_arraysize(bootstrap) - 1;
}

const char *
SDL_GetAudioDriver(int index)
{
    if (index >= 0 && index < SDL_GetNumAudioDrivers()) {
        return bootstrap[index]->name;
    }
    return NULL;
}

int
SDL_AudioInit(const char *driver_name)
{
    int i = 0;
    int initialized = 0;
    int tried_to_init = 0;

    if (SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_AudioQuit();        /* shutdown driver if already running. */
    }

    SDL_zero(current_audio);
    SDL_zero(open_devices);

    /* Select the proper audio driver */
    if (driver_name == NULL) {
        driver_name = SDL_getenv("SDL_AUDIODRIVER");
    }

    for (i = 0; (!initialized) && (bootstrap[i]); ++i) {
        /* make sure we should even try this driver before doing so... */
        const AudioBootStrap *backend = bootstrap[i];
        if ((driver_name && (SDL_strncasecmp(backend->name, driver_name, SDL_strlen(driver_name)) != 0)) ||
            (!driver_name && backend->demand_only)) {
            continue;
        }

        tried_to_init = 1;
        SDL_zero(current_audio);
        current_audio.name = backend->name;
        current_audio.desc = backend->desc;
        initialized = backend->init(&current_audio.impl);
    }

    if (!initialized) {
        /* specific drivers will set the error message if they fail... */
        if (!tried_to_init) {
            if (driver_name) {
                SDL_SetError("Audio target '%s' not available", driver_name);
            } else {
                SDL_SetError("No available audio device");
            }
        }

        SDL_zero(current_audio);
        return -1;            /* No driver was available, so fail. */
    }

    current_audio.detectionLock = SDL_CreateMutex();

    finalize_audio_entry_points();

    /* Make sure we have a list of devices available at startup. */
    current_audio.impl.DetectDevices();

    return 0;
}

/*
 * Get the current audio driver name
 */
const char *
SDL_GetCurrentAudioDriver()
{
    return current_audio.name;
}

/* Clean out devices that we've removed but had to keep around for stability. */
static void
clean_out_device_list(SDL_AudioDeviceItem **devices, int *devCount, SDL_bool *removedFlag)
{
    SDL_AudioDeviceItem *item = *devices;
    SDL_AudioDeviceItem *prev = NULL;
    int total = 0;

    while (item) {
        SDL_AudioDeviceItem *next = item->next;
        if (item->handle != NULL) {
            total++;
            prev = item;
        } else {
            if (prev) {
                prev->next = next;
            } else {
                *devices = next;
            }
            SDL_free(item);
        }
        item = next;
    }

    *devCount = total;
    *removedFlag = SDL_FALSE;
}


int
SDL_GetNumAudioDevices(int iscapture)
{
    int retval = 0;

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        return -1;
    }

    SDL_LockMutex(current_audio.detectionLock);
    if (iscapture && current_audio.captureDevicesRemoved) {
        clean_out_device_list(&current_audio.inputDevices, &current_audio.inputDeviceCount, &current_audio.captureDevicesRemoved);
    }

    if (!iscapture && current_audio.outputDevicesRemoved) {
        clean_out_device_list(&current_audio.outputDevices, &current_audio.outputDeviceCount, &current_audio.outputDevicesRemoved);
        current_audio.outputDevicesRemoved = SDL_FALSE;
    }

    retval = iscapture ? current_audio.inputDeviceCount : current_audio.outputDeviceCount;
    SDL_UnlockMutex(current_audio.detectionLock);

    return retval;
}


const char *
SDL_GetAudioDeviceName(int index, int iscapture)
{
    const char *retval = NULL;

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_SetError("Audio subsystem is not initialized");
        return NULL;
    }

    if ((iscapture) && (!current_audio.impl.HasCaptureSupport)) {
        SDL_SetError("No capture support");
        return NULL;
    }

    if (index >= 0) {
        SDL_AudioDeviceItem *item;
        int i;

        SDL_LockMutex(current_audio.detectionLock);
        item = iscapture ? current_audio.inputDevices : current_audio.outputDevices;
        i = iscapture ? current_audio.inputDeviceCount : current_audio.outputDeviceCount;
        if (index < i) {
            for (i--; i > index; i--, item = item->next) {
                SDL_assert(item != NULL);
            }
            SDL_assert(item != NULL);
            retval = item->name;
        }
        SDL_UnlockMutex(current_audio.detectionLock);
    }

    if (retval == NULL) {
        SDL_SetError("No such device");
    }

    return retval;
}


static void
close_audio_device(SDL_AudioDevice * device)
{
    device->enabled = 0;
    device->shutdown = 1;
    if (device->thread != NULL) {
        SDL_WaitThread(device->thread, NULL);
    }
    if (device->mixer_lock != NULL) {
        SDL_DestroyMutex(device->mixer_lock);
    }
    SDL_FreeAudioMem(device->fake_stream);
    if (device->convert.needed) {
        SDL_FreeAudioMem(device->convert.buf);
    }
    if (device->opened) {
        current_audio.impl.CloseDevice(device);
        device->opened = 0;
    }

    free_audio_queue(device->buffer_queue_head);
    free_audio_queue(device->buffer_queue_pool);

    SDL_FreeAudioMem(device);
}


/*
 * Sanity check desired AudioSpec for SDL_OpenAudio() in (orig).
 *  Fills in a sanitized copy in (prepared).
 *  Returns non-zero if okay, zero on fatal parameters in (orig).
 */
static int
prepare_audiospec(const SDL_AudioSpec * orig, SDL_AudioSpec * prepared)
{
    SDL_memcpy(prepared, orig, sizeof(SDL_AudioSpec));

    if (orig->freq == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_FREQUENCY");
        if ((!env) || ((prepared->freq = SDL_atoi(env)) == 0)) {
            prepared->freq = 22050;     /* a reasonable default */
        }
    }

    if (orig->format == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_FORMAT");
        if ((!env) || ((prepared->format = SDL_ParseAudioFormat(env)) == 0)) {
            prepared->format = AUDIO_S16;       /* a reasonable default */
        }
    }

    switch (orig->channels) {
    case 0:{
            const char *env = SDL_getenv("SDL_AUDIO_CHANNELS");
            if ((!env) || ((prepared->channels = (Uint8) SDL_atoi(env)) == 0)) {
                prepared->channels = 2; /* a reasonable default */
            }
            break;
        }
    case 1:                    /* Mono */
    case 2:                    /* Stereo */
    case 4:                    /* surround */
    case 6:                    /* surround with center and lfe */
        break;
    default:
        SDL_SetError("Unsupported number of audio channels.");
        return 0;
    }

    if (orig->samples == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_SAMPLES");
        if ((!env) || ((prepared->samples = (Uint16) SDL_atoi(env)) == 0)) {
            /* Pick a default of ~46 ms at desired frequency */
            /* !!! FIXME: remove this when the non-Po2 resampling is in. */
            const int samples = (prepared->freq / 1000) * 46;
            int power2 = 1;
            while (power2 < samples) {
                power2 *= 2;
            }
            prepared->samples = power2;
        }
    }

    /* Calculate the silence and size of the audio specification */
    SDL_CalculateAudioSpec(prepared);

    return 1;
}

static SDL_AudioDeviceID
open_audio_device(const char *devname, int iscapture,
                  const SDL_AudioSpec * desired, SDL_AudioSpec * obtained,
                  int allowed_changes, int min_id)
{
    SDL_AudioDeviceID id = 0;
    SDL_AudioSpec _obtained;
    SDL_AudioDevice *device;
    SDL_bool build_cvt;
    void *handle = NULL;
    Uint32 stream_len;
    int i = 0;

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_SetError("Audio subsystem is not initialized");
        return 0;
    }

    if ((iscapture) && (!current_audio.impl.HasCaptureSupport)) {
        SDL_SetError("No capture support");
        return 0;
    }

    /* Find an available device ID... */
    for (id = min_id - 1; id < SDL_arraysize(open_devices); id++) {
        if (open_devices[id] == NULL) {
            break;
        }
    }

    if (id == SDL_arraysize(open_devices)) {
        SDL_SetError("Too many open audio devices");
        return 0;
    }

    if (!obtained) {
        obtained = &_obtained;
    }
    if (!prepare_audiospec(desired, obtained)) {
        return 0;
    }

    /* If app doesn't care about a specific device, let the user override. */
    if (devname == NULL) {
        devname = SDL_getenv("SDL_AUDIO_DEVICE_NAME");
    }

    /*
     * Catch device names at the high level for the simple case...
     * This lets us have a basic "device enumeration" for systems that
     *  don't have multiple devices, but makes sure the device name is
     *  always NULL when it hits the low level.
     *
     * Also make sure that the simple case prevents multiple simultaneous
     *  opens of the default system device.
     */

    if ((iscapture) && (current_audio.impl.OnlyHasDefaultInputDevice)) {
        if ((devname) && (SDL_strcmp(devname, DEFAULT_INPUT_DEVNAME) != 0)) {
            SDL_SetError("No such device");
            return 0;
        }
        devname = NULL;

        for (i = 0; i < SDL_arraysize(open_devices); i++) {
            if ((open_devices[i]) && (open_devices[i]->iscapture)) {
                SDL_SetError("Audio device already open");
                return 0;
            }
        }
    } else if ((!iscapture) && (current_audio.impl.OnlyHasDefaultOutputDevice)) {
        if ((devname) && (SDL_strcmp(devname, DEFAULT_OUTPUT_DEVNAME) != 0)) {
            SDL_SetError("No such device");
            return 0;
        }
        devname = NULL;

        for (i = 0; i < SDL_arraysize(open_devices); i++) {
            if ((open_devices[i]) && (!open_devices[i]->iscapture)) {
                SDL_SetError("Audio device already open");
                return 0;
            }
        }
    } else if (devname != NULL) {
        /* if the app specifies an exact string, we can pass the backend
           an actual device handle thingey, which saves them the effort of
           figuring out what device this was (such as, reenumerating
           everything again to find the matching human-readable name).
           It might still need to open a device based on the string for,
           say, a network audio server, but this optimizes some cases. */
        SDL_AudioDeviceItem *item;
        SDL_LockMutex(current_audio.detectionLock);
        for (item = iscapture ? current_audio.inputDevices : current_audio.outputDevices; item; item = item->next) {
            if ((item->handle != NULL) && (SDL_strcmp(item->name, devname) == 0)) {
                handle = item->handle;
                break;
            }
        }
        SDL_UnlockMutex(current_audio.detectionLock);
    }

    if (!current_audio.impl.AllowsArbitraryDeviceNames) {
        /* has to be in our device list, or the default device. */
        if ((handle == NULL) && (devname != NULL)) {
            SDL_SetError("No such device.");
            return 0;
        }
    }

    device = (SDL_AudioDevice *) SDL_AllocAudioMem(sizeof(SDL_AudioDevice));
    if (device == NULL) {
        SDL_OutOfMemory();
        return 0;
    }
    SDL_zerop(device);
    device->id = id + 1;
    device->spec = *obtained;
    device->enabled = 1;
    device->paused = 1;
    device->iscapture = iscapture;

    /* Create a mutex for locking the sound buffers */
    if (!current_audio.impl.SkipMixerLock) {
        device->mixer_lock = SDL_CreateMutex();
        if (device->mixer_lock == NULL) {
            close_audio_device(device);
            SDL_SetError("Couldn't create mixer lock");
            return 0;
        }
    }

    if (current_audio.impl.OpenDevice(device, handle, devname, iscapture) < 0) {
        close_audio_device(device);
        return 0;
    }
    device->opened = 1;

    /* See if we need to do any conversion */
    build_cvt = SDL_FALSE;
    if (obtained->freq != device->spec.freq) {
        if (allowed_changes & SDL_AUDIO_ALLOW_FREQUENCY_CHANGE) {
            obtained->freq = device->spec.freq;
        } else {
            build_cvt = SDL_TRUE;
        }
    }
    if (obtained->format != device->spec.format) {
        if (allowed_changes & SDL_AUDIO_ALLOW_FORMAT_CHANGE) {
            obtained->format = device->spec.format;
        } else {
            build_cvt = SDL_TRUE;
        }
    }
    if (obtained->channels != device->spec.channels) {
        if (allowed_changes & SDL_AUDIO_ALLOW_CHANNELS_CHANGE) {
            obtained->channels = device->spec.channels;
        } else {
            build_cvt = SDL_TRUE;
        }
    }

    /* If the audio driver changes the buffer size, accept it.
       This needs to be done after the format is modified above,
       otherwise it might not have the correct buffer size.
     */
    if (device->spec.samples != obtained->samples) {
        obtained->samples = device->spec.samples;
        SDL_CalculateAudioSpec(obtained);
    }

    if (build_cvt) {
        /* Build an audio conversion block */
        if (SDL_BuildAudioCVT(&device->convert,
                              obtained->format, obtained->channels,
                              obtained->freq,
                              device->spec.format, device->spec.channels,
                              device->spec.freq) < 0) {
            close_audio_device(device);
            return 0;
        }
        if (device->convert.needed) {
            device->convert.len = (int) (((double) device->spec.size) /
                                         device->convert.len_ratio);

            device->convert.buf =
                (Uint8 *) SDL_AllocAudioMem(device->convert.len *
                                            device->convert.len_mult);
            if (device->convert.buf == NULL) {
                close_audio_device(device);
                SDL_OutOfMemory();
                return 0;
            }
        }
    }

    /* Allocate a fake audio memory buffer */
    stream_len = (device->convert.needed) ? device->convert.len_cvt : 0;
    if (device->spec.size > stream_len) {
        stream_len = device->spec.size;
    }
    SDL_assert(stream_len > 0);
    device->fake_stream = (Uint8 *)SDL_AllocAudioMem(stream_len);
    if (device->fake_stream == NULL) {
        close_audio_device(device);
        SDL_OutOfMemory();
        return 0;
    }

    if (device->spec.callback == NULL) {  /* use buffer queueing? */
        /* pool a few packets to start. Enough for two callbacks. */
        const int packetlen = SDL_AUDIOBUFFERQUEUE_PACKETLEN;
        const int wantbytes = ((device->convert.needed) ? device->convert.len : device->spec.size) * 2;
        const int wantpackets = (wantbytes / packetlen) + ((wantbytes % packetlen) ? packetlen : 0);
        for (i = 0; i < wantpackets; i++) {
            SDL_AudioBufferQueue *packet = (SDL_AudioBufferQueue *) SDL_malloc(sizeof (SDL_AudioBufferQueue));
            if (packet) { /* don't care if this fails, we'll deal later. */
                packet->datalen = 0;
                packet->startpos = 0;
                packet->next = device->buffer_queue_pool;
                device->buffer_queue_pool = packet;
            }
        }

        device->spec.callback = SDL_BufferQueueDrainCallback;
        device->spec.userdata = device;
    }

    /* add it to our list of open devices. */
    open_devices[id] = device;

    /* Start the audio thread if necessary */
    if (!current_audio.impl.ProvidesOwnCallbackThread) {
        /* Start the audio thread */
        char name[64];
        SDL_snprintf(name, sizeof (name), "SDLAudioDev%d", (int) device->id);
/* !!! FIXME: this is nasty. */
#if defined(__WIN32__) && !defined(HAVE_LIBC)
#undef SDL_CreateThread
#if SDL_DYNAMIC_API
        device->thread = SDL_CreateThread_REAL(SDL_RunAudio, name, device, NULL, NULL);
#else
        device->thread = SDL_CreateThread(SDL_RunAudio, name, device, NULL, NULL);
#endif
#else
        device->thread = SDL_CreateThread(SDL_RunAudio, name, device);
#endif
        if (device->thread == NULL) {
            SDL_CloseAudioDevice(device->id);
            SDL_SetError("Couldn't create audio thread");
            return 0;
        }
    }

    return device->id;
}


int
SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained)
{
    SDL_AudioDeviceID id = 0;

    /* Start up the audio driver, if necessary. This is legacy behaviour! */
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            return -1;
        }
    }

    /* SDL_OpenAudio() is legacy and can only act on Device ID #1. */
    if (open_devices[0] != NULL) {
        SDL_SetError("Audio device is already opened");
        return -1;
    }

    if (obtained) {
        id = open_audio_device(NULL, 0, desired, obtained,
                               SDL_AUDIO_ALLOW_ANY_CHANGE, 1);
    } else {
        id = open_audio_device(NULL, 0, desired, NULL, 0, 1);
    }

    SDL_assert((id == 0) || (id == 1));
    return (id == 0) ? -1 : 0;
}

SDL_AudioDeviceID
SDL_OpenAudioDevice(const char *device, int iscapture,
                    const SDL_AudioSpec * desired, SDL_AudioSpec * obtained,
                    int allowed_changes)
{
    return open_audio_device(device, iscapture, desired, obtained,
                             allowed_changes, 2);
}

SDL_AudioStatus
SDL_GetAudioDeviceStatus(SDL_AudioDeviceID devid)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    SDL_AudioStatus status = SDL_AUDIO_STOPPED;
    if (device && device->enabled) {
        if (device->paused) {
            status = SDL_AUDIO_PAUSED;
        } else {
            status = SDL_AUDIO_PLAYING;
        }
    }
    return status;
}


SDL_AudioStatus
SDL_GetAudioStatus(void)
{
    return SDL_GetAudioDeviceStatus(1);
}

void
SDL_PauseAudioDevice(SDL_AudioDeviceID devid, int pause_on)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        current_audio.impl.LockDevice(device);
        device->paused = pause_on;
        current_audio.impl.UnlockDevice(device);
    }
}

void
SDL_PauseAudio(int pause_on)
{
    SDL_PauseAudioDevice(1, pause_on);
}


void
SDL_LockAudioDevice(SDL_AudioDeviceID devid)
{
    /* Obtain a lock on the mixing buffers */
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        current_audio.impl.LockDevice(device);
    }
}

void
SDL_LockAudio(void)
{
    SDL_LockAudioDevice(1);
}

void
SDL_UnlockAudioDevice(SDL_AudioDeviceID devid)
{
    /* Obtain a lock on the mixing buffers */
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        current_audio.impl.UnlockDevice(device);
    }
}

void
SDL_UnlockAudio(void)
{
    SDL_UnlockAudioDevice(1);
}

void
SDL_CloseAudioDevice(SDL_AudioDeviceID devid)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        close_audio_device(device);
        open_devices[devid - 1] = NULL;
    }
}

void
SDL_CloseAudio(void)
{
    SDL_CloseAudioDevice(1);
}

void
SDL_AudioQuit(void)
{
    SDL_AudioDeviceID i;

    if (!current_audio.name) {  /* not initialized?! */
        return;
    }

    for (i = 0; i < SDL_arraysize(open_devices); i++) {
        if (open_devices[i] != NULL) {
            SDL_CloseAudioDevice(i+1);
        }
    }

    free_device_list(&current_audio.outputDevices, &current_audio.outputDeviceCount);
    free_device_list(&current_audio.inputDevices, &current_audio.inputDeviceCount);

    /* Free the driver data */
    current_audio.impl.Deinitialize();

    SDL_DestroyMutex(current_audio.detectionLock);

    SDL_zero(current_audio);
    SDL_zero(open_devices);
}

#define NUM_FORMATS 10
static int format_idx;
static int format_idx_sub;
static SDL_AudioFormat format_list[NUM_FORMATS][NUM_FORMATS] = {
    {AUDIO_U8, AUDIO_S8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S8, AUDIO_U8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
};

SDL_AudioFormat
SDL_FirstAudioFormat(SDL_AudioFormat format)
{
    for (format_idx = 0; format_idx < NUM_FORMATS; ++format_idx) {
        if (format_list[format_idx][0] == format) {
            break;
        }
    }
    format_idx_sub = 0;
    return SDL_NextAudioFormat();
}

SDL_AudioFormat
SDL_NextAudioFormat(void)
{
    if ((format_idx == NUM_FORMATS) || (format_idx_sub == NUM_FORMATS)) {
        return 0;
    }
    return format_list[format_idx][format_idx_sub++];
}

void
SDL_CalculateAudioSpec(SDL_AudioSpec * spec)
{
    switch (spec->format) {
    case AUDIO_U8:
        spec->silence = 0x80;
        break;
    default:
        spec->silence = 0x00;
        break;
    }
    spec->size = SDL_AUDIO_BITSIZE(spec->format) / 8;
    spec->size *= spec->channels;
    spec->size *= spec->samples;
}


/*
 * Moved here from SDL_mixer.c, since it relies on internals of an opened
 *  audio device (and is deprecated, by the way!).
 */
void
SDL_MixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume)
{
    /* Mix the user-level audio format */
    SDL_AudioDevice *device = get_audio_device(1);
    if (device != NULL) {
        SDL_AudioFormat format;
        if (device->convert.needed) {
            format = device->convert.src_format;
        } else {
            format = device->spec.format;
        }
        SDL_MixAudioFormat(dst, src, format, len, volume);
    }
}

/* vi: set ts=4 sw=4 expandtab: */
