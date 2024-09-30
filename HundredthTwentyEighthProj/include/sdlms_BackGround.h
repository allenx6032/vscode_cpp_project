#pragma once
#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class BackGround : public Entity
{
public:
    BackGround(wz::Node *node, int id, World *world);
    ~BackGround();

    enum
    {
        NORMAL,
        HTILED,
        VTILED,
        TILED,
        HMOVEA,
        VMOVEA,
        HMOVEB,
        VMOVEB
    };
};
