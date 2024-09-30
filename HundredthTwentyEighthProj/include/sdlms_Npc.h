#pragma once

#include "sdlms_Entity.h"
#include "wzlib_Property.hpp"
#include "sdlms_World.h"
#include "sdlms_AnimatedSprite.h"

class Npc : public Entity
{
public:
    Npc(wz::Node *node, int id, int rx0, int rx1, World *world);
    ~Npc();

    void switch_act(const std::u16string &action);
    void stop_animate();
    void start_animate();

private:
    std::unordered_map<std::u16string, AnimatedSprite *> aspr_map;
    std::unordered_map<std::u16string, std::u16string> str_map;
    std::u16string act = u"";
};
