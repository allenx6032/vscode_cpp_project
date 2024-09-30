#pragma once
#include <ft2_build.h>
#include FT_FREETYPE_H

#include "sdlms_Sprite.h"

class FreeType
{
public:
    static void init(const std::string &filename_prefix = "");
    static void size(int size);
    static inline FT_Library *library;
    static inline FT_Face *face;
    static Sprite *str(const std::u16string &s, SDL_Color color = {255, 255, 255, 255}, int w = 0);
};