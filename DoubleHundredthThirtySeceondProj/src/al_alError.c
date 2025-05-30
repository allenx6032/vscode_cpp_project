/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2000 by authors.
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
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "al_config.h"

#include <signal.h>

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "al_alMain.h"
#include "al_alc.h"
#include "al_alError.h"

ALboolean TrapALError = AL_FALSE;

ALvoid alSetError(ALCcontext *Context, ALenum errorCode)
{
    ALenum curerr = AL_NO_ERROR;

    WARN("Error generated on context %p, code 0x%04x\n", Context, errorCode);
    if(TrapALError)
    {
#ifdef _WIN32
        /* DebugBreak will cause an exception if there is no debugger */
        if(IsDebuggerPresent())
            DebugBreak();
#elif defined(SIGTRAP)
        raise(SIGTRAP);
#endif
    }

    (void)(ATOMIC_COMPARE_EXCHANGE_STRONG_SEQ(&Context->LastError, &curerr, errorCode));
}

AL_API ALenum AL_APIENTRY alGetError(void)
{
    ALCcontext *Context;
    ALenum errorCode;

    Context = GetContextRef();
    if(!Context)
    {
        WARN("Querying error state on null context (implicitly 0x%04x)\n",
             AL_INVALID_OPERATION);
        if(TrapALError)
        {
#ifdef _WIN32
            if(IsDebuggerPresent())
                DebugBreak();
#elif defined(SIGTRAP)
            raise(SIGTRAP);
#endif
        }
        return AL_INVALID_OPERATION;
    }

    errorCode = ATOMIC_EXCHANGE_SEQ(&Context->LastError, AL_NO_ERROR);

    ALCcontext_DecRef(Context);

    return errorCode;
}
