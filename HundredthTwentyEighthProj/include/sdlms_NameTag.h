#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class NameTag : public Entity
{
public:
    NameTag(int width, int height);
    NameTag(int width, int height, const std::u16string &val);
    int get_width();
    int get_height();

    ~NameTag();
};
