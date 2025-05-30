/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "faur_palette.v.h"
#include <faur.v.h>

static FPalette* newPalette(const FPixels* Pixels)
{
    if(Pixels->size.x < 1 || Pixels->size.y < 2) {
        F__FATAL("Invalid palette size %dx%d", Pixels->size.x, Pixels->size.y);
    }

    // First pixel sets the non-palette color to ignore; row 0 is reserved
    FColorPixel ignore = f_pixels__bufferGetValue(Pixels, 0, 0, 0);
    const FColorPixel* palPixels =
        f_pixels__bufferGetFromConst(Pixels, 0, 0, 1);

    unsigned num = 0;
    int bufferLen = Pixels->size.x * (Pixels->size.y - 1);

    for(int i = bufferLen; i--; ) {
        if(*palPixels++ != ignore) {
            num++;
        }
    }

    if(num == 0) {
        F__FATAL("Palette has no colors");
    }

    FPalette* p = f_mem_malloc(
                    sizeof(FPalette) + (num - 1) * sizeof(FPaletteEntry));

    p->size = num;

    palPixels = f_pixels__bufferGetFromConst(Pixels, 0, 0, 1);

    for(int i = bufferLen, color = 0; i--; ) {
        FColorPixel pixel = *palPixels++;

        if(pixel != ignore) {
            p->entries[color].pixel = pixel;
            p->entries[color].rgb = f_color_pixelToRgb(pixel);

            if(++color == (int)num) {
                break;
            }
        }
    }

    return p;
}

FPalette* f_palette_newFromFile(const char* Path)
{
    F__CHECK(Path != NULL);

    FPixels* pixels = f_platform_api__imageRead(Path);

    if(pixels == NULL) {
        F__FATAL("f_palette_newFromFile(%s): Cannot open file", Path);
    }

    FPalette* palette = newPalette(pixels);

    f_pixels__free(pixels);

    return palette;
}

FPalette* f_palette_newFromSprite(const FSprite* Sprite)
{
    F__CHECK(Sprite != NULL);

    return newPalette(&Sprite->pixels);
}

void f_palette_free(FPalette* Palette)
{
    if(Palette == NULL) {
        return;
    }

    f_mem_free(Palette);
}

FColorPixel f_palette_getPixel(const FPalette* Palette, unsigned Index)
{
    F__CHECK(Palette != NULL);
    F__CHECK(Index < Palette->size);

    return Palette->entries[Index].pixel;
}

FColorRgb f_palette_getRgb(const FPalette* Palette, unsigned Index)
{
    F__CHECK(Palette != NULL);
    F__CHECK(Index < Palette->size);

    return Palette->entries[Index].rgb;
}

unsigned f_palette_sizeGet(const FPalette* Palette)
{
    F__CHECK(Palette != NULL);

    return Palette->size;
}
