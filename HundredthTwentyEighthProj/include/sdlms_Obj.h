#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class Obj : public Entity
{
public:
    Obj(wz::Node *node,int id,int layer, World *world);
    ~Obj();
};
