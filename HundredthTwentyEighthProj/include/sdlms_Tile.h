#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class Tile : public Entity
{
public:
    Tile(wz::Node *node, const std::u16string &ts, int layer, int id, World *world);
    ~Tile();
};
