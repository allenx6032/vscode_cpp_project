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
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "al_config.h"

#include <stdlib.h>

#include "al_alMain.h"
#include "al_alThunk.h"

#include "al_almalloc.h"


static ATOMIC_FLAG *ThunkArray;
static ALsizei      ThunkArraySize;
static RWLock ThunkLock;

void ThunkInit(void)
{
    RWLockInit(&ThunkLock);
    ThunkArraySize = 1024;
    ThunkArray = al_calloc(16, ThunkArraySize * sizeof(*ThunkArray));
}

void ThunkExit(void)
{
    al_free(ThunkArray);
    ThunkArray = NULL;
    ThunkArraySize = 0;
}

ALenum NewThunkEntry(ALuint *index)
{
    void *NewList;
    ALsizei i;

    ReadLock(&ThunkLock);
    for(i = 0;i < ThunkArraySize;i++)
    {
        if(!ATOMIC_FLAG_TEST_AND_SET(&ThunkArray[i], almemory_order_acq_rel))
        {
            ReadUnlock(&ThunkLock);
            *index = i+1;
            return AL_NO_ERROR;
        }
    }
    ReadUnlock(&ThunkLock);

    WriteLock(&ThunkLock);
    /* Double-check that there's still no free entries, in case another
     * invocation just came through and increased the size of the array.
     */
    for(;i < ThunkArraySize;i++)
    {
        if(!ATOMIC_FLAG_TEST_AND_SET(&ThunkArray[i], almemory_order_acq_rel))
        {
            WriteUnlock(&ThunkLock);
            *index = i+1;
            return AL_NO_ERROR;
        }
    }

    NewList = al_calloc(16, ThunkArraySize*2 * sizeof(*ThunkArray));
    if(!NewList)
    {
        WriteUnlock(&ThunkLock);
        ERR("Realloc failed to increase to %u entries!\n", ThunkArraySize*2);
        return AL_OUT_OF_MEMORY;
    }
    memcpy(NewList, ThunkArray, ThunkArraySize*sizeof(*ThunkArray));
    al_free(ThunkArray);
    ThunkArray = NewList;
    ThunkArraySize *= 2;

    ATOMIC_FLAG_TEST_AND_SET(&ThunkArray[i], almemory_order_seq_cst);
    *index = ++i;

    for(;i < ThunkArraySize;i++)
        ATOMIC_FLAG_CLEAR(&ThunkArray[i], almemory_order_relaxed);
    WriteUnlock(&ThunkLock);

    return AL_NO_ERROR;
}

void FreeThunkEntry(ALuint index)
{
    ReadLock(&ThunkLock);
    if(index > 0 && (ALsizei)index <= ThunkArraySize)
        ATOMIC_FLAG_CLEAR(&ThunkArray[index-1], almemory_order_release);
    ReadUnlock(&ThunkLock);
}
