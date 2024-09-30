#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"
#include "sdlms_Sprite.h"

class ChatBalloon : public Entity
{
public:
    ChatBalloon(int width, int height, const std::u16string &val);
    ~ChatBalloon();
    int get_width();
    int get_height();

public:
    Sprite *spr;
};
