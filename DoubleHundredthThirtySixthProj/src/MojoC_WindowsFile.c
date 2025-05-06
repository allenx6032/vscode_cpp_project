/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2013-8-29
 * Update   : 2019-1-8
 * Author   : scott.cgi
 */
 

#include "MojoC_Platform.h"


//------------------------
#ifdef IS_PLATFORM_WINDOWS
//------------------------


#include <string.h>
#include "MojoC_File.h"
#include "MojoC_Log.h"


static File* Open(const char* resourceFilePath)
{
    AAsset* asset = AAssetManager_open(nativeActivity->assetManager, resourceFilePath, AASSET_MODE_UNKNOWN);
    return (File*) asset;
}


static int OpenFileDescriptor(const char* resourceFilePath, long* outStart, long* outLength)
{
    AAsset* asset = AAssetManager_open(nativeActivity->assetManager, resourceFilePath, AASSET_MODE_UNKNOWN);
    int     fd    = AAsset_openFileDescriptor(asset, (off_t*) outStart, (off_t*) outLength);
    AAsset_close(asset);

    return fd;
}


static void Close(File* file)
{
    AAsset_close((AAsset*) file);
}


static long GetLength(File* file)
{
    return (long) AAsset_getLength((AAsset*) file);
}


static int Read(File* file, void* buffer, size_t count)
{
    return AAsset_read((AAsset*) file, buffer, count);
}


static long Seek(File* file, long offset, int whence)
{
    return (long) AAsset_seek((AAsset*) file, (off_t) offset, whence);
}


static const char* GetInternalDataPath(int* outPathLength)
{
    static int length = -1;

    if (length == -1)
    {
        length = (int) strlen(nativeActivity->internalDataPath);
    }

    if (outPathLength != NULL)
    {
        *outPathLength = length;
    }

    return nativeActivity->internalDataPath;
}


struct AFile AFile[1] =
{{
    Open,
    OpenFileDescriptor,
    Close,
    GetLength,
    Read,
    Seek,
    GetInternalDataPath,
}};


//---------------------------
#endif // IS_PLATFORM_ANDROID
//---------------------------

