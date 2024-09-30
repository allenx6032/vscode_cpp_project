#pragma once
#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class LadderRope : public Entity
{
public:
    LadderRope(wz::Node *node, World *world);
    ~LadderRope();

public:
    // lr的附加属性
    int l;
    int uf;
    int page;
};