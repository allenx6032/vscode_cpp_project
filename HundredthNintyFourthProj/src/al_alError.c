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
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "al_config.h"

#include "al_alMain.h"
#include "al_alc.h"
#include "al_alError.h"

AL_API ALenum AL_APIENTRY alGetError(ALvoid)
{
    ALCcontext *Context;
    ALenum errorCode;

    Context = GetContextSuspended();
    if(!Context) return AL_INVALID_OPERATION;

    errorCode = Context->LastError;
    Context->LastError = AL_NO_ERROR;

    ProcessContext(Context);

    return errorCode;
}

ALvoid alSetError(ALCcontext *Context, ALenum errorCode)
{
    if(Context->LastError == AL_NO_ERROR)
        Context->LastError = errorCode;
}
