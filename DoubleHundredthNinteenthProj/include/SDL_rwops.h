/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

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
 *  \file SDL_rwops.h
 *
 *  This file provides a general interface for SDL to read and write
 *  data streams.  It can easily be extended to files, memory, etc.
 */

#ifndef SDL_rwops_h_
#define SDL_rwops_h_

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "SDL_begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* RWops Types */
#define SDL_RWOPS_UNKNOWN   0U  /**< Unknown stream type */
#define SDL_RWOPS_WINFILE   1U  /**< Win32 file */
#define SDL_RWOPS_STDFILE   2U  /**< Stdio file */
#define SDL_RWOPS_JNIFILE   3U  /**< Android asset */
#define SDL_RWOPS_MEMORY    4U  /**< Memory stream */
#define SDL_RWOPS_MEMORY_RO 5U  /**< Read-Only memory stream */

/**
 * This is the read/write operation structure -- very basic.
 */
typedef struct SDL_RWops
{
    /**
     *  Return the size of the file in this rwops, or -1 if unknown
     */
    Sint64 (SDLCALL * size) (struct SDL_RWops * context);

    /**
     *  Seek to \c offset relative to \c whence, one of stdio's whence values:
     *  RW_SEEK_SET, RW_SEEK_CUR, RW_SEEK_END
     *
     *  \return the final offset in the data stream, or -1 on error.
     */
    Sint64 (SDLCALL * seek) (struct SDL_RWops * context, Sint64 offset,
                             int whence);

    /**
     *  Read up to \c maxnum objects each of size \c size from the data
     *  stream to the area pointed at by \c ptr.
     *
     *  \return the number of objects read, or 0 at error or end of file.
     */
    size_t (SDLCALL * read) (struct SDL_RWops * context, void *ptr,
                             size_t size, size_t maxnum);

    /**
     *  Write exactly \c num objects each of size \c size from the area
     *  pointed at by \c ptr to data stream.
     *
     *  \return the number of objects written, or 0 at error or end of file.
     */
    size_t (SDLCALL * write) (struct SDL_RWops * context, const void *ptr,
                              size_t size, size_t num);

    /**
     *  Close and free an allocated SDL_RWops structure.
     *
     *  \return 0 if successful or -1 on write error when flushing data.
     */
    int (SDLCALL * close) (struct SDL_RWops * context);

    Uint32 type;
    union
    {
#if defined(__ANDROID__)
        struct
        {
            void *asset;
        } androidio;
#elif defined(__WIN32__)
        struct
        {
            SDL_bool append;
            void *h;
            struct
            {
                void *data;
                size_t size;
                size_t left;
            } buffer;
        } windowsio;
#endif

#ifdef HAVE_STDIO_H
        struct
        {
            SDL_bool autoclose;
            FILE *fp;
        } stdio;
#endif
        struct
        {
            Uint8 *base;
            Uint8 *here;
            Uint8 *stop;
        } mem;
        struct
        {
            void *data1;
            void *data2;
        } unknown;
    } hidden;

} SDL_RWops;


/**
 *  \name RWFrom functions
 *
 *  Functions to create SDL_RWops structures from various data streams.
 */
/* @{ */

extern DECLSPEC SDL_RWops *SDLCALL SDL_RWFromFile(const char *file,
                                                  const char *mode);

#ifdef HAVE_STDIO_H
extern DECLSPEC SDL_RWops *SDLCALL SDL_RWFromFP(FILE * fp,
                                                SDL_bool autoclose);
#else
extern DECLSPEC SDL_RWops *SDLCALL SDL_RWFromFP(void * fp,
                                                SDL_bool autoclose);
#endif

extern DECLSPEC SDL_RWops *SDLCALL SDL_RWFromMem(void *mem, int size);
extern DECLSPEC SDL_RWops *SDLCALL SDL_RWFromConstMem(const void *mem,
                                                      int size);

/* @} *//* RWFrom functions */


extern DECLSPEC SDL_RWops *SDLCALL SDL_AllocRW(void);
extern DECLSPEC void SDLCALL SDL_FreeRW(SDL_RWops * area);

#define RW_SEEK_SET 0       /**< Seek from the beginning of data */
#define RW_SEEK_CUR 1       /**< Seek relative to current read point */
#define RW_SEEK_END 2       /**< Seek relative to the end of data */

/**
 *  Return the size of the file in this rwops, or -1 if unknown
 */
extern DECLSPEC Sint64 SDLCALL SDL_RWsize(SDL_RWops *context);

/**
 *  Seek to \c offset relative to \c whence, one of stdio's whence values:
 *  RW_SEEK_SET, RW_SEEK_CUR, RW_SEEK_END
 *
 *  \return the final offset in the data stream, or -1 on error.
 */
extern DECLSPEC Sint64 SDLCALL SDL_RWseek(SDL_RWops *context,
                                          Sint64 offset, int whence);

/**
 *  Return the current offset in the data stream, or -1 on error.
 */
extern DECLSPEC Sint64 SDLCALL SDL_RWtell(SDL_RWops *context);

/**
 *  Read up to \c maxnum objects each of size \c size from the data
 *  stream to the area pointed at by \c ptr.
 *
 *  \return the number of objects read, or 0 at error or end of file.
 */
extern DECLSPEC size_t SDLCALL SDL_RWread(SDL_RWops *context,
                                          void *ptr, size_t size, size_t maxnum);

/**
 *  Write exactly \c num objects each of size \c size from the area
 *  pointed at by \c ptr to data stream.
 *
 *  \return the number of objects written, or 0 at error or end of file.
 */
extern DECLSPEC size_t SDLCALL SDL_RWwrite(SDL_RWops *context,
                                           const void *ptr, size_t size, size_t num);

/**
 *  Close and free an allocated SDL_RWops structure.
 *
 *  \return 0 if successful or -1 on write error when flushing data.
 */
extern DECLSPEC int SDLCALL SDL_RWclose(SDL_RWops *context);

/**
 *  Load all the data from an SDL data stream.
 *
 *  The data is allocated with a zero byte at the end (null terminated)
 *
 *  If \c datasize is not NULL, it is filled with the size of the data read.
 *
 *  If \c freesrc is non-zero, the stream will be closed after being read.
 *
 *  The data should be freed with SDL_free().
 *
 *  \return the data, or NULL if there was an error.
 */
extern DECLSPEC void *SDLCALL SDL_LoadFile_RW(SDL_RWops * src, size_t *datasize,
                                                    int freesrc);

/**
 *  Load an entire file.
 *
 *  The data is allocated with a zero byte at the end (null terminated)
 *
 *  If \c datasize is not NULL, it is filled with the size of the data read.
 *
 *  If \c freesrc is non-zero, the stream will be closed after being read.
 *
 *  The data should be freed with SDL_free().
 *
 *  \return the data, or NULL if there was an error.
 */
extern DECLSPEC void *SDLCALL SDL_LoadFile(const char *file, size_t *datasize);

/**
 *  \name Read endian functions
 *
 *  Read an item of the specified endianness and return in native format.
 */
/* @{ */
extern DECLSPEC Uint8 SDLCALL SDL_ReadU8(SDL_RWops * src);
extern DECLSPEC Uint16 SDLCALL SDL_ReadLE16(SDL_RWops * src);
extern DECLSPEC Uint16 SDLCALL SDL_ReadBE16(SDL_RWops * src);
extern DECLSPEC Uint32 SDLCALL SDL_ReadLE32(SDL_RWops * src);
extern DECLSPEC Uint32 SDLCALL SDL_ReadBE32(SDL_RWops * src);
extern DECLSPEC Uint64 SDLCALL SDL_ReadLE64(SDL_RWops * src);
extern DECLSPEC Uint64 SDLCALL SDL_ReadBE64(SDL_RWops * src);
/* @} *//* Read endian functions */

/**
 *  \name Write endian functions
 *
 *  Write an item of native format to the specified endianness.
 */
/* @{ */
extern DECLSPEC size_t SDLCALL SDL_WriteU8(SDL_RWops * dst, Uint8 value);
extern DECLSPEC size_t SDLCALL SDL_WriteLE16(SDL_RWops * dst, Uint16 value);
extern DECLSPEC size_t SDLCALL SDL_WriteBE16(SDL_RWops * dst, Uint16 value);
extern DECLSPEC size_t SDLCALL SDL_WriteLE32(SDL_RWops * dst, Uint32 value);
extern DECLSPEC size_t SDLCALL SDL_WriteBE32(SDL_RWops * dst, Uint32 value);
extern DECLSPEC size_t SDLCALL SDL_WriteLE64(SDL_RWops * dst, Uint64 value);
extern DECLSPEC size_t SDLCALL SDL_WriteBE64(SDL_RWops * dst, Uint64 value);
/* @} *//* Write endian functions */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "SDL_close_code.h"

#endif /* SDL_rwops_h_ */

/* vi: set ts=4 sw=4 expandtab: */
