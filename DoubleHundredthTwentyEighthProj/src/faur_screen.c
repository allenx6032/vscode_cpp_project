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

#include "faur_screen.v.h"
#include <faur.v.h>

FScreen f__screen;
static F_LISTINTR(g_stack, FScreen, listNode);

#if F_CONFIG_TRAIT_DESKTOP && F_CONFIG_TRAIT_KEYBOARD
    static FButton* g_fullScreenButton;

    #define F__ZOOM_LEVELS 4
    static FButton* g_zoomButtons[F__ZOOM_LEVELS];
#endif

static void f_screen__init(void)
{
    FVecInt size = f_platform_api__screenSizeGet();

    f__screen.pixels = f_platform_api__screenPixelsGet();
    f__screen.texture = f_platform_api__screenTextureGet();
    f__screen.clipStart = (FVecInt){0, 0};
    f__screen.clipEnd = size;
    f__screen.clipSize = size;

    #if F_CONFIG_TRAIT_DESKTOP && F_CONFIG_TRAIT_KEYBOARD
        g_fullScreenButton = f_button_new();
        f_button_bindKey(g_fullScreenButton, F_KEY_F11);

        for(int z = 0; z < F__ZOOM_LEVELS; z++) {
            g_zoomButtons[z] = f_button_new();
            f_button_bindKey(g_zoomButtons[z], F_KEY_F1 + z);
        }
    #endif
}

static void f_screen__uninit(void)
{
    f_listintr_apply(&g_stack, f_pool_release);

    #if F_CONFIG_TRAIT_DESKTOP && F_CONFIG_TRAIT_KEYBOARD
        f_button_free(g_fullScreenButton);

        for(int z = 0; z < F__ZOOM_LEVELS; z++) {
            f_button_free(g_zoomButtons[z]);
        }
    #endif
}

const FPack f_pack__screen = {
    "Screen",
    f_screen__init,
    f_screen__uninit,
};

void f_screen__tick(void)
{
    #if F_CONFIG_TRAIT_DESKTOP && F_CONFIG_TRAIT_KEYBOARD
        if(f_button_pressGetOnce(g_fullScreenButton)) {
            f_platform_api__screenFullscreenFlip();

            f_out__info("Screen is now %s",
                        f_platform_api__screenFullscreenGet()
                            ? "fullscreen" : "windowed");
        }

        for(int z = 0; z < F__ZOOM_LEVELS; z++) {
            if(f_button_pressGetOnce(g_zoomButtons[z])) {
                int zoom = z + 1;

                if(f_platform_api__screenZoomGet() != zoom) {
                    f_platform_api__screenZoomSet(zoom);

                    f_out__info(
                        "Screen zoom %d", f_platform_api__screenZoomGet());
                }

                break;
            }
        }
    #endif
}

void f_screen__draw(void)
{
    if(!f_listintr_sizeIsEmpty(&g_stack)) {
        F__FATAL("Screen target stack is not empty");
    }

    f_platform_api__screenShow();
}

FColorPixel* f_screen_pixelsGetBuffer(void)
{
    f_platform_api__screenTextureSync();

    return f_screen__bufferGetFrom(0, 0);
}

FVecInt f_screen_sizeGet(void)
{
    return f__screen.pixels->size;
}

int f_screen_sizeGetWidth(void)
{
    return f__screen.pixels->size.x;
}

int f_screen_sizeGetHeight(void)
{
    return f__screen.pixels->size.y;
}

void f_screen_clear(void)
{
    f_platform_api__screenClear();

    f_pixels__fill(f__screen.pixels, f__screen.frame, f__color.pixel);
}

void f_screen_push(FSprite* Sprite, unsigned Frame)
{
    F__CHECK(Sprite != NULL);
    F__CHECK(Frame < Sprite->pixels.framesNum);

    f_listintr_push(&g_stack, f_pool__dup(F_POOL__STACK_SCREEN, &f__screen));

    f__screen.pixels = &Sprite->pixels;
    f__screen.sprite = Sprite;
    f__screen.frame = Frame;

    f__screen.yOffset = (int)f__screen.frame * f__screen.pixels->size.y;
    f__screen.texture =
        f_platform_api__textureSpriteToScreen(Sprite->u.texture);

    f_platform_api__screenTextureSet(f__screen.texture);

    f_screen_clipReset();
}

void f_screen_pop(void)
{
    FScreen* screen = f_listintr_pop(&g_stack);

    if(screen == NULL) {
        F__FATAL("f_screen_pop: Stack is empty");
    }

    f_platform_api__textureUpdate(f__screen.sprite->u.texture,
                                  &f__screen.sprite->pixels,
                                  f__screen.frame);

    f__screen = *screen;
    f_pool_release(screen);

    f_platform_api__screenTextureSet(f__screen.texture);
    f_platform_api__screenClipSet();
}

void f_screen_clipSet(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    if(!f_screen_boxInsideScreen(X, Y, Width, Height)) {
        f_out__error(
            "f_screen_clipSet(%d, %d, %d, %d): Invalid area on %dx%d screen",
            X,
            Y,
            Width,
            Height,
            f__screen.pixels->size.x,
            f__screen.pixels->size.y);

        return;
    }

    f__screen.clipStart = (FVecInt){X, Y};
    f__screen.clipEnd = (FVecInt){X + Width, Y + Height};
    f__screen.clipSize = (FVecInt){Width, Height};

    f_platform_api__screenClipSet();
}

void f_screen_clipReset(void)
{
    f_screen_clipSet(0, 0, f__screen.pixels->size.x, f__screen.pixels->size.y);
}

bool f_screen_boxOnScreen(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    return f_collide_boxAndBox((FVecInt){X, Y},
                               (FVecInt){Width, Height},
                               (FVecInt){0, 0},
                               f__screen.pixels->size);
}

bool f_screen_boxInsideScreen(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    return X >= 0 && Y >= 0
        && X + Width <= f__screen.pixels->size.x
        && Y + Height <= f__screen.pixels->size.y;
}

bool f_screen_boxOnClip(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    return f_collide_boxAndBox((FVecInt){X, Y},
                               (FVecInt){Width, Height},
                               f__screen.clipStart,
                               f__screen.clipSize);
}

bool f_screen_boxInsideClip(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    return X >= f__screen.clipStart.x && Y >= f__screen.clipStart.y
        && X + Width <= f__screen.clipEnd.x && Y + Height <= f__screen.clipEnd.y;
}

void f_screen__toSprite(FSprite* Sprite, unsigned Frame)
{
    FVecInt spriteSize = f_sprite_sizeGet(Sprite);

    if(!f_vecint_equal(f__screen.pixels->size, spriteSize)) {
        F__FATAL("f_screen__toSprite: %dx%d sprite, %dx%d screen",
                 spriteSize.x,
                 spriteSize.y,
                 f__screen.pixels->size.x,
                 f__screen.pixels->size.y);
    }

    #if F_CONFIG_SCREEN_RENDER == F_SCREEN_RENDER_SOFTWARE
        f_pixels__copyFrame(
            &Sprite->pixels, Frame, f__screen.pixels, f__screen.frame);
    #endif

    f_platform_api__screenToTexture(
        f_platform_api__textureSpriteToScreen(Sprite->u.texture), Frame);
}
