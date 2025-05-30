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

/*
   Code to load and save surfaces in Windows BMP format.

   Why support BMP format?  Well, it's a native format for Windows, and
   most image processing programs can read and write it.  It would be nice
   to be able to have at least one image format that we can natively load
   and save, and since PNG is so complex that it would bloat the library,
   BMP is a good alternative.

   This code currently supports Win32 DIBs in uncompressed 8 and 24 bpp.
*/

#include "SDL_video.h"
#include "SDL_assert.h"
#include "SDL_endian.h"
#include "SDL_pixels_c.h"

#define SAVE_32BIT_BMP

/* Compression encodings for BMP files */
#ifndef BI_RGB
#define BI_RGB      0
#define BI_RLE8     1
#define BI_RLE4     2
#define BI_BITFIELDS    3
#endif


static void CorrectAlphaChannel(SDL_Surface *surface)
{
    /* Check to see if there is any alpha channel data */
    SDL_bool hasAlpha = SDL_FALSE;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int alphaChannelOffset = 0;
#else
    int alphaChannelOffset = 3;
#endif
    Uint8 *alpha = ((Uint8*)surface->pixels) + alphaChannelOffset;
    Uint8 *end = alpha + surface->h * surface->pitch;

    while (alpha < end) {
        if (*alpha != 0) {
            hasAlpha = SDL_TRUE;
            break;
        }
        alpha += 4;
    }

    if (!hasAlpha) {
        alpha = ((Uint8*)surface->pixels) + alphaChannelOffset;
        while (alpha < end) {
            *alpha = SDL_ALPHA_OPAQUE;
            alpha += 4;
        }
    }
}

SDL_Surface *
SDL_LoadBMP_RW(SDL_RWops * src, int freesrc)
{
    SDL_bool was_error;
    Sint64 fp_offset = 0;
    int bmpPitch;
    int i, pad;
    SDL_Surface *surface;
    Uint32 Rmask = 0;
    Uint32 Gmask = 0;
    Uint32 Bmask = 0;
    Uint32 Amask = 0;
    SDL_Palette *palette;
    Uint8 *bits;
    Uint8 *top, *end;
    SDL_bool topDown;
    int ExpandBMP;
    SDL_bool haveRGBMasks = SDL_FALSE;
    SDL_bool haveAlphaMask = SDL_FALSE;
    SDL_bool correctAlpha = SDL_FALSE;

    /* The Win32 BMP file header (14 bytes) */
    char magic[2];
    /* Uint32 bfSize = 0; */
    /* Uint16 bfReserved1 = 0; */
    /* Uint16 bfReserved2 = 0; */
    Uint32 bfOffBits = 0;

    /* The Win32 BITMAPINFOHEADER struct (40 bytes) */
    Uint32 biSize = 0;
    Sint32 biWidth = 0;
    Sint32 biHeight = 0;
    /* Uint16 biPlanes = 0; */
    Uint16 biBitCount = 0;
    Uint32 biCompression = 0;
    /* Uint32 biSizeImage = 0; */
    /* Sint32 biXPelsPerMeter = 0; */
    /* Sint32 biYPelsPerMeter = 0; */
    Uint32 biClrUsed = 0;
    /* Uint32 biClrImportant = 0; */

    /* Make sure we are passed a valid data source */
    surface = NULL;
    was_error = SDL_FALSE;
    if (src == NULL) {
        was_error = SDL_TRUE;
        goto done;
    }

    /* Read in the BMP file header */
    fp_offset = SDL_RWtell(src);
    SDL_ClearError();
    if (SDL_RWread(src, magic, 1, 2) != 2) {
        SDL_Error(SDL_EFREAD);
        was_error = SDL_TRUE;
        goto done;
    }
    if (SDL_strncmp(magic, "BM", 2) != 0) {
        SDL_SetError("File is not a Windows BMP file");
        was_error = SDL_TRUE;
        goto done;
    }
    /* bfSize = */ SDL_ReadLE32(src);
    /* bfReserved1 = */ SDL_ReadLE16(src);
    /* bfReserved2 = */ SDL_ReadLE16(src);
    bfOffBits = SDL_ReadLE32(src);

    /* Read the Win32 BITMAPINFOHEADER */
    biSize = SDL_ReadLE32(src);
    if (biSize == 12) {   /* really old BITMAPCOREHEADER */
        biWidth = (Uint32) SDL_ReadLE16(src);
        biHeight = (Uint32) SDL_ReadLE16(src);
        /* biPlanes = */ SDL_ReadLE16(src);
        biBitCount = SDL_ReadLE16(src);
        biCompression = BI_RGB;
    } else if (biSize >= 40) {  /* some version of BITMAPINFOHEADER */
        Uint32 headerSize;
        biWidth = SDL_ReadLE32(src);
        biHeight = SDL_ReadLE32(src);
        /* biPlanes = */ SDL_ReadLE16(src);
        biBitCount = SDL_ReadLE16(src);
        biCompression = SDL_ReadLE32(src);
        /* biSizeImage = */ SDL_ReadLE32(src);
        /* biXPelsPerMeter = */ SDL_ReadLE32(src);
        /* biYPelsPerMeter = */ SDL_ReadLE32(src);
        biClrUsed = SDL_ReadLE32(src);
        /* biClrImportant = */ SDL_ReadLE32(src);

        /* 64 == BITMAPCOREHEADER2, an incompatible OS/2 2.x extension. Skip this stuff for now. */
        if (biSize == 64) {
            /* ignore these extra fields. */
            if (biCompression == BI_BITFIELDS) {
                /* this value is actually huffman compression in this variant. */
                SDL_SetError("Compressed BMP files not supported");
                was_error = SDL_TRUE;
                goto done;
            }
        } else {
            /* This is complicated. If compression is BI_BITFIELDS, then
               we have 3 DWORDS that specify the RGB masks. This is either
               stored here in an BITMAPV2INFOHEADER (which only differs in
               that it adds these RGB masks) and biSize >= 52, or we've got
               these masks stored in the exact same place, but strictly
               speaking, this is the bmiColors field in BITMAPINFO immediately
               following the legacy v1 info header, just past biSize. */
            if (biCompression == BI_BITFIELDS) {
                haveRGBMasks = SDL_TRUE;
                Rmask = SDL_ReadLE32(src);
                Gmask = SDL_ReadLE32(src);
                Bmask = SDL_ReadLE32(src);

                /* ...v3 adds an alpha mask. */
                if (biSize >= 56) {  /* BITMAPV3INFOHEADER; adds alpha mask */
                    haveAlphaMask = SDL_TRUE;
                    Amask = SDL_ReadLE32(src);
                }
            } else {
                /* the mask fields are ignored for v2+ headers if not BI_BITFIELD. */
                if (biSize >= 52) {  /* BITMAPV2INFOHEADER; adds RGB masks */
                    /*Rmask = */ SDL_ReadLE32(src);
                    /*Gmask = */ SDL_ReadLE32(src);
                    /*Bmask = */ SDL_ReadLE32(src);
                }
                if (biSize >= 56) {  /* BITMAPV3INFOHEADER; adds alpha mask */
                    /*Amask = */ SDL_ReadLE32(src);
                }
            }

            /* Insert other fields here; Wikipedia and MSDN say we're up to
               v5 of this header, but we ignore those for now (they add gamma,
               color spaces, etc). Ignoring the weird OS/2 2.x format, we
               currently parse up to v3 correctly (hopefully!). */
        }

        /* skip any header bytes we didn't handle... */
        headerSize = (Uint32) (SDL_RWtell(src) - (fp_offset + 14));
        if (biSize > headerSize) {
            SDL_RWseek(src, (biSize - headerSize), RW_SEEK_CUR);
        }
    }
    if (biHeight < 0) {
        topDown = SDL_TRUE;
        biHeight = -biHeight;
    } else {
        topDown = SDL_FALSE;
    }

    /* Check for read error */
    if (SDL_strcmp(SDL_GetError(), "") != 0) {
        was_error = SDL_TRUE;
        goto done;
    }

    /* Expand 1 and 4 bit bitmaps to 8 bits per pixel */
    switch (biBitCount) {
    case 1:
    case 4:
        ExpandBMP = biBitCount;
        biBitCount = 8;
        break;
    default:
        ExpandBMP = 0;
        break;
    }

    /* We don't support any BMP compression right now */
    switch (biCompression) {
    case BI_RGB:
        /* If there are no masks, use the defaults */
        SDL_assert(!haveRGBMasks);
        SDL_assert(!haveAlphaMask);
        /* Default values for the BMP format */
        switch (biBitCount) {
        case 15:
        case 16:
            Rmask = 0x7C00;
            Gmask = 0x03E0;
            Bmask = 0x001F;
            break;
        case 24:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            Rmask = 0x000000FF;
            Gmask = 0x0000FF00;
            Bmask = 0x00FF0000;
#else
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
#endif
            break;
        case 32:
            /* We don't know if this has alpha channel or not */
            correctAlpha = SDL_TRUE;
            Amask = 0xFF000000;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;
        default:
            break;
        }
        break;

    case BI_BITFIELDS:
        break;  /* we handled this in the info header. */

    default:
        SDL_SetError("Compressed BMP files not supported");
        was_error = SDL_TRUE;
        goto done;
    }

    /* Create a compatible surface, note that the colors are RGB ordered */
    surface =
        SDL_CreateRGBSurface(0, biWidth, biHeight, biBitCount, Rmask, Gmask,
                             Bmask, Amask);
    if (surface == NULL) {
        was_error = SDL_TRUE;
        goto done;
    }

    /* Load the palette, if any */
    palette = (surface->format)->palette;
    if (palette) {
        SDL_assert(biBitCount <= 8);
        if (biClrUsed == 0) {
            biClrUsed = 1 << biBitCount;
        }
        if ((int) biClrUsed > palette->ncolors) {
            SDL_Color *colors;
            int ncolors = biClrUsed;
            colors =
                (SDL_Color *) SDL_realloc(palette->colors,
                                          ncolors *
                                          sizeof(*palette->colors));
            if (!colors) {
                SDL_OutOfMemory();
                was_error = SDL_TRUE;
                goto done;
            }
            palette->ncolors = ncolors;
            palette->colors = colors;
        } else if ((int) biClrUsed < palette->ncolors) {
            palette->ncolors = biClrUsed;
        }
        if (biSize == 12) {
            for (i = 0; i < (int) biClrUsed; ++i) {
                SDL_RWread(src, &palette->colors[i].b, 1, 1);
                SDL_RWread(src, &palette->colors[i].g, 1, 1);
                SDL_RWread(src, &palette->colors[i].r, 1, 1);
                palette->colors[i].a = SDL_ALPHA_OPAQUE;
            }
        } else {
            for (i = 0; i < (int) biClrUsed; ++i) {
                SDL_RWread(src, &palette->colors[i].b, 1, 1);
                SDL_RWread(src, &palette->colors[i].g, 1, 1);
                SDL_RWread(src, &palette->colors[i].r, 1, 1);
                SDL_RWread(src, &palette->colors[i].a, 1, 1);

                /* According to Microsoft documentation, the fourth element
                   is reserved and must be zero, so we shouldn't treat it as
                   alpha.
                */
                palette->colors[i].a = SDL_ALPHA_OPAQUE;
            }
        }
    }

    /* Read the surface pixels.  Note that the bmp image is upside down */
    if (SDL_RWseek(src, fp_offset + bfOffBits, RW_SEEK_SET) < 0) {
        SDL_Error(SDL_EFSEEK);
        was_error = SDL_TRUE;
        goto done;
    }
    top = (Uint8 *)surface->pixels;
    end = (Uint8 *)surface->pixels+(surface->h*surface->pitch);
    switch (ExpandBMP) {
    case 1:
        bmpPitch = (biWidth + 7) >> 3;
        pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
        break;
    case 4:
        bmpPitch = (biWidth + 1) >> 1;
        pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
        break;
    default:
        pad = ((surface->pitch % 4) ? (4 - (surface->pitch % 4)) : 0);
        break;
    }
    if (topDown) {
        bits = top;
    } else {
        bits = end - surface->pitch;
    }
    while (bits >= top && bits < end) {
        switch (ExpandBMP) {
        case 1:
        case 4:{
                Uint8 pixel = 0;
                int shift = (8 - ExpandBMP);
                for (i = 0; i < surface->w; ++i) {
                    if (i % (8 / ExpandBMP) == 0) {
                        if (!SDL_RWread(src, &pixel, 1, 1)) {
                            SDL_SetError("Error reading from BMP");
                            was_error = SDL_TRUE;
                            goto done;
                        }
                    }
                    *(bits + i) = (pixel >> shift);
                    pixel <<= ExpandBMP;
                }
            }
            break;

        default:
            if (SDL_RWread(src, bits, 1, surface->pitch)
                != surface->pitch) {
                SDL_Error(SDL_EFREAD);
                was_error = SDL_TRUE;
                goto done;
            }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            /* Byte-swap the pixels if needed. Note that the 24bpp
               case has already been taken care of above. */
            switch (biBitCount) {
            case 15:
            case 16:{
                    Uint16 *pix = (Uint16 *) bits;
                    for (i = 0; i < surface->w; i++)
                        pix[i] = SDL_Swap16(pix[i]);
                    break;
                }

            case 32:{
                    Uint32 *pix = (Uint32 *) bits;
                    for (i = 0; i < surface->w; i++)
                        pix[i] = SDL_Swap32(pix[i]);
                    break;
                }
            }
#endif
            break;
        }
        /* Skip padding bytes, ugh */
        if (pad) {
            Uint8 padbyte;
            for (i = 0; i < pad; ++i) {
                SDL_RWread(src, &padbyte, 1, 1);
            }
        }
        if (topDown) {
            bits += surface->pitch;
        } else {
            bits -= surface->pitch;
        }
    }
    if (correctAlpha) {
        CorrectAlphaChannel(surface);
    }
  done:
    if (was_error) {
        if (src) {
            SDL_RWseek(src, fp_offset, RW_SEEK_SET);
        }
        SDL_FreeSurface(surface);
        surface = NULL;
    }
    if (freesrc && src) {
        SDL_RWclose(src);
    }
    return (surface);
}

int
SDL_SaveBMP_RW(SDL_Surface * saveme, SDL_RWops * dst, int freedst)
{
    Sint64 fp_offset;
    int i, pad;
    SDL_Surface *surface;
    Uint8 *bits;

    /* The Win32 BMP file header (14 bytes) */
    char magic[2] = { 'B', 'M' };
    Uint32 bfSize;
    Uint16 bfReserved1;
    Uint16 bfReserved2;
    Uint32 bfOffBits;

    /* The Win32 BITMAPINFOHEADER struct (40 bytes) */
    Uint32 biSize;
    Sint32 biWidth;
    Sint32 biHeight;
    Uint16 biPlanes;
    Uint16 biBitCount;
    Uint32 biCompression;
    Uint32 biSizeImage;
    Sint32 biXPelsPerMeter;
    Sint32 biYPelsPerMeter;
    Uint32 biClrUsed;
    Uint32 biClrImportant;

    /* Make sure we have somewhere to save */
    surface = NULL;
    if (dst) {
        SDL_bool save32bit = SDL_FALSE;
#ifdef SAVE_32BIT_BMP
        /* We can save alpha information in a 32-bit BMP */
        if (saveme->map->info.flags & SDL_COPY_COLORKEY ||
            saveme->format->Amask) {
            save32bit = SDL_TRUE;
        }
#endif /* SAVE_32BIT_BMP */

        if (saveme->format->palette && !save32bit) {
            if (saveme->format->BitsPerPixel == 8) {
                surface = saveme;
            } else {
                SDL_SetError("%d bpp BMP files not supported",
                             saveme->format->BitsPerPixel);
            }
        } else if ((saveme->format->BitsPerPixel == 24) &&
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                   (saveme->format->Rmask == 0x00FF0000) &&
                   (saveme->format->Gmask == 0x0000FF00) &&
                   (saveme->format->Bmask == 0x000000FF)
#else
                   (saveme->format->Rmask == 0x000000FF) &&
                   (saveme->format->Gmask == 0x0000FF00) &&
                   (saveme->format->Bmask == 0x00FF0000)
#endif
            ) {
            surface = saveme;
        } else {
            SDL_PixelFormat format;

            /* If the surface has a colorkey or alpha channel we'll save a
               32-bit BMP with alpha channel, otherwise save a 24-bit BMP. */
            if (save32bit) {
                SDL_InitFormat(&format,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                               SDL_PIXELFORMAT_ARGB8888
#else
                               SDL_PIXELFORMAT_BGRA8888
#endif
                               );
            } else {
                SDL_InitFormat(&format, SDL_PIXELFORMAT_BGR24);
            }
            surface = SDL_ConvertSurface(saveme, &format, 0);
            if (!surface) {
                SDL_SetError("Couldn't convert image to %d bpp",
                             format.BitsPerPixel);
            }
        }
    } else {
        /* Set no error here because it may overwrite a more useful message from
           SDL_RWFromFile() if SDL_SaveBMP_RW() is called from SDL_SaveBMP(). */
        return -1;
    }

    if (surface && (SDL_LockSurface(surface) == 0)) {
        const int bw = surface->w * surface->format->BytesPerPixel;

        /* Set the BMP file header values */
        bfSize = 0;             /* We'll write this when we're done */
        bfReserved1 = 0;
        bfReserved2 = 0;
        bfOffBits = 0;          /* We'll write this when we're done */

        /* Write the BMP file header values */
        fp_offset = SDL_RWtell(dst);
        SDL_ClearError();
        SDL_RWwrite(dst, magic, 2, 1);
        SDL_WriteLE32(dst, bfSize);
        SDL_WriteLE16(dst, bfReserved1);
        SDL_WriteLE16(dst, bfReserved2);
        SDL_WriteLE32(dst, bfOffBits);

        /* Set the BMP info values */
        biSize = 40;
        biWidth = surface->w;
        biHeight = surface->h;
        biPlanes = 1;
        biBitCount = surface->format->BitsPerPixel;
        biCompression = BI_RGB;
        biSizeImage = surface->h * surface->pitch;
        biXPelsPerMeter = 0;
        biYPelsPerMeter = 0;
        if (surface->format->palette) {
            biClrUsed = surface->format->palette->ncolors;
        } else {
            biClrUsed = 0;
        }
        biClrImportant = 0;

        /* Write the BMP info values */
        SDL_WriteLE32(dst, biSize);
        SDL_WriteLE32(dst, biWidth);
        SDL_WriteLE32(dst, biHeight);
        SDL_WriteLE16(dst, biPlanes);
        SDL_WriteLE16(dst, biBitCount);
        SDL_WriteLE32(dst, biCompression);
        SDL_WriteLE32(dst, biSizeImage);
        SDL_WriteLE32(dst, biXPelsPerMeter);
        SDL_WriteLE32(dst, biYPelsPerMeter);
        SDL_WriteLE32(dst, biClrUsed);
        SDL_WriteLE32(dst, biClrImportant);

        /* Write the palette (in BGR color order) */
        if (surface->format->palette) {
            SDL_Color *colors;
            int ncolors;

            colors = surface->format->palette->colors;
            ncolors = surface->format->palette->ncolors;
            for (i = 0; i < ncolors; ++i) {
                SDL_RWwrite(dst, &colors[i].b, 1, 1);
                SDL_RWwrite(dst, &colors[i].g, 1, 1);
                SDL_RWwrite(dst, &colors[i].r, 1, 1);
                SDL_RWwrite(dst, &colors[i].a, 1, 1);
            }
        }

        /* Write the bitmap offset */
        bfOffBits = (Uint32)(SDL_RWtell(dst) - fp_offset);
        if (SDL_RWseek(dst, fp_offset + 10, RW_SEEK_SET) < 0) {
            SDL_Error(SDL_EFSEEK);
        }
        SDL_WriteLE32(dst, bfOffBits);
        if (SDL_RWseek(dst, fp_offset + bfOffBits, RW_SEEK_SET) < 0) {
            SDL_Error(SDL_EFSEEK);
        }

        /* Write the bitmap image upside down */
        bits = (Uint8 *) surface->pixels + (surface->h * surface->pitch);
        pad = ((bw % 4) ? (4 - (bw % 4)) : 0);
        while (bits > (Uint8 *) surface->pixels) {
            bits -= surface->pitch;
            if (SDL_RWwrite(dst, bits, 1, bw) != bw) {
                SDL_Error(SDL_EFWRITE);
                break;
            }
            if (pad) {
                const Uint8 padbyte = 0;
                for (i = 0; i < pad; ++i) {
                    SDL_RWwrite(dst, &padbyte, 1, 1);
                }
            }
        }

        /* Write the BMP file size */
        bfSize = (Uint32)(SDL_RWtell(dst) - fp_offset);
        if (SDL_RWseek(dst, fp_offset + 2, RW_SEEK_SET) < 0) {
            SDL_Error(SDL_EFSEEK);
        }
        SDL_WriteLE32(dst, bfSize);
        if (SDL_RWseek(dst, fp_offset + bfSize, RW_SEEK_SET) < 0) {
            SDL_Error(SDL_EFSEEK);
        }

        /* Close it up.. */
        SDL_UnlockSurface(surface);
        if (surface != saveme) {
            SDL_FreeSurface(surface);
        }
    }

    if (freedst && dst) {
        SDL_RWclose(dst);
    }
    return ((SDL_strcmp(SDL_GetError(), "") == 0) ? 0 : -1);
}

/* vi: set ts=4 sw=4 expandtab: */
