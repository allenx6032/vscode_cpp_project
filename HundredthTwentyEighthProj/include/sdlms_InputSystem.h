#pragma once
#include "sdlms_System.h"
#include "sdlms_Transform.h"
#include "sdlms_Normal.h"
#include "sdlms_Player.h"
#include "sdlms_RandomInput.h"

class InputSystem : public System
{
public:
	void run(World &world) override;

private:
	void update_player(Player *nor, World &world);
	void update_random_input(RandomInput *ran, World &world);
};
