#pragma once
#include "sdlms_Entity.h"
#include "sdlms_World.h"

class Character : public Entity
{
public:
    Character(World *world, SDL_FPoint p);
    ~Character();

    void switch_act(uint8_t action);
    void stop_animate();
    void start_animate();
};
