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

/**
 *  \file SDL_platform.h
 *
 *  Try to get a standard set of platform defines.
 */

#ifndef _SDL_platform_h
#define _SDL_platform_h

#if defined(_AIX)
#undef __AIX__
#define __AIX__     1
#endif
#if defined(__HAIKU__)
#undef __HAIKU__
#define __HAIKU__   1
#endif
#if defined(bsdi) || defined(__bsdi) || defined(__bsdi__)
#undef __BSDI__
#define __BSDI__    1
#endif
#if defined(_arch_dreamcast)
#undef __DREAMCAST__
#define __DREAMCAST__   1
#endif
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__DragonFly__)
#undef __FREEBSD__
#define __FREEBSD__ 1
#endif
#if defined(hpux) || defined(__hpux) || defined(__hpux__)
#undef __HPUX__
#define __HPUX__    1
#endif
#if defined(sgi) || defined(__sgi) || defined(__sgi__) || defined(_SGI_SOURCE)
#undef __IRIX__
#define __IRIX__    1
#endif
#if (defined(linux) || defined(__linux) || defined(__linux__))
#undef __LINUX__
#define __LINUX__   1
#endif
#if defined(ANDROID) || defined(__ANDROID__)
#undef __ANDROID__
#undef __LINUX__ /* do we need to do this? */
#define __ANDROID__ 1
#endif

#if defined(__APPLE__)
/* lets us know what version of Mac OS X we're compiling on */
#include "AvailabilityMacros.h"
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
/* if compiling for iPhone */
#undef __IPHONEOS__
#define __IPHONEOS__ 1
#undef __MACOSX__
#else
/* if not compiling for iPhone */
#undef __MACOSX__
#define __MACOSX__  1
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1050
# error SDL for Mac OS X only supports deploying on 10.5 and above.
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < 1050 */
#endif /* TARGET_OS_IPHONE */
#endif /* defined(__APPLE__) */

#if defined(__NetBSD__)
#undef __NETBSD__
#define __NETBSD__  1
#endif
#if defined(__OpenBSD__)
#undef __OPENBSD__
#define __OPENBSD__ 1
#endif
#if defined(__OS2__)
#undef __OS2__
#define __OS2__     1
#endif
#if defined(osf) || defined(__osf) || defined(__osf__) || defined(_OSF_SOURCE)
#undef __OSF__
#define __OSF__     1
#endif
#if defined(__QNXNTO__)
#undef __QNXNTO__
#define __QNXNTO__  1
#endif
#if defined(riscos) || defined(__riscos) || defined(__riscos__)
#undef __RISCOS__
#define __RISCOS__  1
#endif
#if defined(__sun) && defined(__SVR4)
#undef __SOLARIS__
#define __SOLARIS__ 1
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
/* Try to find out if we're compiling for WinRT or non-WinRT */
/* If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset. */
#if (defined(_MSC_VER) && (_MSC_VER >= 1700) && !_USING_V110_SDK71_)	/* _MSC_VER==1700 for MSVC 2012 */
#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#undef __WINDOWS__
#define __WINDOWS__   1
/* See if we're compiling for WinRT: */
#elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#undef __WINRT__
#define __WINRT__ 1
#endif
#else
#undef __WINDOWS__
#define __WINDOWS__   1
#endif /* _MSC_VER < 1700 */
#endif /* defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) */

#if defined(__WINDOWS__)
#undef __WIN32__
#define __WIN32__ 1
#endif
#if defined(__PSP__)
#undef __PSP__
#define __PSP__ 1
#endif

/* The NACL compiler defines __native_client__ and __pnacl__
 * Ref: http://www.chromium.org/nativeclient/pnacl/stability-of-the-pnacl-bitcode-abi
 */
#if defined(__native_client__)
#undef __LINUX__
#undef __NACL__
#define __NACL__ 1
#endif
#if defined(__pnacl__)
#undef __LINUX__
#undef __PNACL__
#define __PNACL__ 1
/* PNACL with newlib supports static linking only */
#define __SDL_NOGETPROCADDR__
#endif


#include "SDL_begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Gets the name of the platform.
 */
extern DECLSPEC const char * SDLCALL SDL_GetPlatform (void);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "SDL_close_code.h"

#endif /* _SDL_platform_h */

/* vi: set ts=4 sw=4 expandtab: */
