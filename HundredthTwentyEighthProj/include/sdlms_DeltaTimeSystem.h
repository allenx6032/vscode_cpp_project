#pragma once
#include "sdlms_System.h"
#include "sdlms_AnimatedSprite.h"
#include "sdlms_Transform.h"
#include "sdlms_HVMove.h"
#include "sdlms_Avatar.h"
#include "sdlms_Video.h"

#include "sdlms_Timer.h"

class DeltaTimeSystem : public System
{
public:
	void run(World &world) override;

private:
	bool update_animated_sprite(AnimatedSprite *aspr, int delta_time, World &world);
	void update_avatar(Avatar *hvm, int delta_time, World &world);
	void update_timer(int delta_time, World &world);
};
