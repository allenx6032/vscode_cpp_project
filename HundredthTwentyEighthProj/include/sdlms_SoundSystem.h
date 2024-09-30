#pragma once
#include "sdlms_System.h"
#include "sdlms_Sound.h"

class SoundSystem : public System
{
public:
	void run(World &world) override;

private:
	static int play_sound(void *sou);
};
