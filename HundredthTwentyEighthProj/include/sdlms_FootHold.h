#pragma once
#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"

class FootHold : public Entity
{
public:
    FootHold(wz::Node *node,int page, int zmass, World *world);
    ~FootHold();

public:
    constexpr auto get_page() { return page; }

public:
    // fh的附加属性
    int page;
    int zmass;
    int prev;
    int next;
};
