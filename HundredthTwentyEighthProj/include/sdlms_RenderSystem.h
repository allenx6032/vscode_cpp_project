#pragma once
#include "sdlms_System.h"
#include "sdlms_Sprite.h"
#include "sdlms_AnimatedSprite.h"
#include "sdlms_Transform.h"
#include "sdlms_HVTile.h"
#include "sdlms_HVMove.h"
#include "sdlms_Avatar.h"
#include "sdlms_Video.h"
#include "sdlms_Npc.h"
#include "sdlms_FootHold.h"
#include "sdlms_Line.h"

class RenderSystem : public System
{
public:
	void run(World &world) override;

private:
	void render_sprite(Transform *tr, Sprite *spr, World &world);
	void render_animated_sprite(Transform *tr, AnimatedSprite *aspr, World &world);
	void render_hvtile_sprite(Transform *tr, HVTile *hvt, World &world);
	void render_avatar_sprite(Transform *tr, Avatar *ava, World &world);
	void render_video(Transform *tr,Video *vid, World &world);
	void render_fh(FootHold *fh,World &world);
};
