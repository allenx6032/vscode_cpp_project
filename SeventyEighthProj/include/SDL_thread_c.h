/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

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

#ifndef _SDL_thread_c_h
#define _SDL_thread_c_h

#include "SDL_thread.h"

/* Need the definitions of SYS_ThreadHandle */
#if SDL_THREADS_DISABLED
#include "generic/SDL_systhread_c.h"
#elif SDL_THREAD_PTHREAD
#include "pthread/SDL_systhread_c.h"
#elif SDL_THREAD_WINDOWS
#include "SDL_systhread_c.h"
#elif SDL_THREAD_PSP
#include "psp/SDL_systhread_c.h"
#elif SDL_THREAD_STDCPP
#include "stdcpp/SDL_systhread_c.h"
#else
#error Need thread implementation for this platform
#include "generic/SDL_systhread_c.h"
#endif
#include "SDL_error_c.h"

typedef enum SDL_ThreadState
{
    SDL_THREAD_STATE_ALIVE,
    SDL_THREAD_STATE_DETACHED,
    SDL_THREAD_STATE_ZOMBIE,
    SDL_THREAD_STATE_CLEANED,
} SDL_ThreadState;

/* This is the system-independent thread info structure */
struct SDL_Thread
{
    SDL_threadID threadid;
    SYS_ThreadHandle handle;
    int status;
    SDL_atomic_t state;  /* SDL_THREAD_STATE_* */
    SDL_error errbuf;
    char *name;
    size_t stacksize;  /* 0 for default, >0 for user-specified stack size. */
    void *data;
};

/* This is the function called to run a thread */
extern void SDL_RunThread(void *data);

/* This is the system-independent thread local storage structure */
typedef struct {
    unsigned int limit;
    struct {
        void *data;
        void (*destructor)(void*);
    } array[1];
} SDL_TLSData;

/* This is how many TLS entries we allocate at once */
#define TLS_ALLOC_CHUNKSIZE 4

/* Get cross-platform, slow, thread local storage for this thread.
   This is only intended as a fallback if getting real thread-local
   storage fails or isn't supported on this platform.
 */
extern SDL_TLSData *SDL_Generic_GetTLSData();

/* Set cross-platform, slow, thread local storage for this thread.
   This is only intended as a fallback if getting real thread-local
   storage fails or isn't supported on this platform.
 */
extern int SDL_Generic_SetTLSData(SDL_TLSData *data);

#endif /* _SDL_thread_c_h */

/* vi: set ts=4 sw=4 expandtab: */
