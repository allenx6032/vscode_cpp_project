#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

#include "sdlms_FreeType.hpp"

class String : public Entity
{
public:
    String(const std::u16string &s, SDL_Color color = {255, 255, 255, 255}, int w = 0, int size = 12);
    int get_width();
    int get_height();
    ~String();

public:
    Sprite *spr;
};
