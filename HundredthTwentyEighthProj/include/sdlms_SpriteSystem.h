#pragma once
#include "sdlms_System.h"
#include "sdlms_DistanceSprite.h"
#include "sdlms_Transform.h"

class SpriteSystem : public System
{
public:
	void run(World &world) override;

private:
	void update_dis(DistanceSprite *dis, World &world);
};
