/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "faur_color.v.h"
#include <faur.v.h>

// #include <faur_v/faur_gfx/f_palette.png.h>

FColorState f__color;

FColorPixel f_color__key;
FColorPixel f_color__limit;

static F_LISTINTR(g_stack, FColorState, listNode);
static FPalette* g_palette;

static void f_color__init(void)
{
    g_palette = f_palette_newFromSprite(FSprite_f_palette);

    f_color_reset();

    f_color__key = f_color_pixelFromHex(F_CONFIG_COLOR_SPRITE_KEY);
    f_color__limit = f_color_pixelFromHex(F_CONFIG_COLOR_SPRITE_BORDER);
}

static void f_color__uninit(void)
{
    f_listintr_apply(&g_stack, f_pool_release);
    f_palette_free(g_palette);
}

const FPack f_pack__color = {
    "Color",
    f_color__init,
    f_color__uninit,
};

void f_color_push(void)
{
    f_listintr_push(&g_stack, f_pool__dup(F_POOL__STACK_COLOR, &f__color));

    f_color_reset();
}

void f_color_pop(void)
{
    FColorState* state = f_listintr_pop(&g_stack);

    if(state == NULL) {
        F__FATAL("f_color_pop: Stack is empty");
    }

    f__color = *state;
    f_pool_release(state);

    f_color_blendSet(f__color.blend);
    f_color_colorSetRgba(
        f__color.rgb.r, f__color.rgb.g, f__color.rgb.b, f__color.alpha);
}

void f_color_reset(void)
{
    f_color_blendSet(F_COLOR_BLEND_SOLID);
    f_color_colorSetRgba(0, 0, 0, F_COLOR_ALPHA_MAX);
    f_color_fillBlitSet(false);
    f_color_fillDrawSet(true);
}

void f_color_paletteSet(const FPalette* Palette)
{
    F__CHECK(Palette != NULL);

    f__color.palette = Palette;
}

#if F__OPTIMIZE_ALPHA
static void optimizeAlphaBlending(void)
{
    if(f__color.canonicalBlend == F_COLOR_BLEND_ALPHA) {
        FColorBlend fastestBlend = F_COLOR_BLEND_ALPHA;

        switch(f__color.alpha) {
            case F_COLOR_ALPHA_MAX / 4: {
                fastestBlend = F_COLOR_BLEND_ALPHA_25;
            } break;

            case F_COLOR_ALPHA_MAX / 2: {
                fastestBlend = F_COLOR_BLEND_ALPHA_50;
            } break;

            case F_COLOR_ALPHA_MAX * 3 / 4: {
                fastestBlend = F_COLOR_BLEND_ALPHA_75;
            } break;

            case F_COLOR_ALPHA_MAX: {
                fastestBlend = F_COLOR_BLEND_SOLID;
            } break;
        }

        if(f__color.blend != fastestBlend) {
            f__color.blend = fastestBlend;
        }
    }
}
#endif

void f_color_blendSet(FColorBlend Blend)
{
    F__CHECK(Blend < F_COLOR_BLEND_NUM);

    f__color.blend = Blend;

    #if F__OPTIMIZE_ALPHA
        f__color.canonicalBlend = Blend;
        optimizeAlphaBlending();
    #else
        if(Blend == F_COLOR_BLEND_ALPHA_25) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX / 4);
        } else if(Blend == F_COLOR_BLEND_ALPHA_50) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX / 2);
        } else if(Blend == F_COLOR_BLEND_ALPHA_75) {
            f_color_alphaSet(F_COLOR_ALPHA_MAX * 3 / 4);
        }
    #endif

    f_platform_api__drawSetBlend();
}

void f_color_alphaSet(int Alpha)
{
    F__CHECK(Alpha >= 0 && Alpha <= F_COLOR_ALPHA_MAX);

    f__color.alpha = Alpha;

    #if F__OPTIMIZE_ALPHA
        optimizeAlphaBlending();
    #endif

    f_platform_api__drawSetColor();
}

static inline void setRgb(int Red, int Green, int Blue)
{
    f__color.rgb.r = Red;
    f__color.rgb.g = Green;
    f__color.rgb.b = Blue;
    f__color.pixel = f_color_pixelFromRgb3(Red, Green, Blue);
}

void f_color_colorSetRgb(int Red, int Green, int Blue)
{
    F__CHECK(Red >= 0 && Red <= 255);
    F__CHECK(Green >= 0 && Green <= 255);
    F__CHECK(Blue >= 0 && Blue <= 255);

    setRgb(Red, Green, Blue);

    f_platform_api__drawSetColor();
}

void f_color_colorSetRgba(int Red, int Green, int Blue, int Alpha)
{
    F__CHECK(Red >= 0 && Red <= 255);
    F__CHECK(Green >= 0 && Green <= 255);
    F__CHECK(Blue >= 0 && Blue <= 255);
    F__CHECK(Alpha >= 0 && Alpha <= F_COLOR_ALPHA_MAX);

    setRgb(Red, Green, Blue);
    f__color.alpha = Alpha;

    #if F__OPTIMIZE_ALPHA
        optimizeAlphaBlending();
    #endif

    f_platform_api__drawSetColor();
}

void f_color_colorSetHex(uint32_t Hexcode)
{
    f__color.rgb.r = (Hexcode >> 16) & 0xff;
    f__color.rgb.g = (Hexcode >> 8)  & 0xff;
    f__color.rgb.b = (Hexcode)       & 0xff;
    f__color.pixel = f_color_pixelFromHex(Hexcode);

    f_platform_api__drawSetColor();
}

void f_color_colorSetPixel(FColorPixel Pixel)
{
    f__color.rgb = f_color_pixelToRgb(Pixel);
    f__color.pixel = Pixel;

    f_platform_api__drawSetColor();
}

void f_color_colorSetIndex(unsigned ColorIndex)
{
    F__CHECK(f__color.palette != NULL);

    f_color_colorSetPixel(f_palette_getPixel(f__color.palette, ColorIndex));
}

void f_color__colorSetInternal(FColorPaletteInternal ColorIndex)
{
    f_color_colorSetPixel(f_palette_getPixel(g_palette, (unsigned)ColorIndex));
}

void f_color_fillBlitSet(bool Fill)
{
    f__color.fillBlit = Fill;
}

void f_color_fillDrawSet(bool Fill)
{
    f__color.fillDraw = Fill;
}
